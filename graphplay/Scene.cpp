// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <algorithm>

#include "graphplay.h"
#include "Scene.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace graphplay {
    Scene::Scene(unsigned int vp_width, unsigned int vp_height)
        : m_vp_width(vp_width),
          m_vp_height(vp_height),
          m_camera(),
          m_projection(),
          m_lights(),
          m_meshes(),
          m_view_projection_uniform_buffer(0),
          m_light_uniform_buffer(0),
          m_uniform_buffer_bindings()
    {
        m_uniform_buffer_bindings["view_and_projection"] = 0;
        m_uniform_buffer_bindings["light_list"] = 1;

        for (auto i = 0; i < MAX_LIGHTS; ++i) {
            m_lights[i].enabled = false;
        }

        m_lights[0].enabled = true;
        m_lights[0].position = glm::vec3(0.0, 10.0, 10.0);
        m_lights[0].color = glm::vec4(1.0, 1.0, 1.0, 1.0);
        m_lights[0].specular_exp = 4;

        // m_lights[1].enabled = true;
        // m_lights[1].position = glm::vec3(10.0, 10.0, 0.0);
        // m_lights[1].color = glm::vec4(1.0, 0.0, 0.0, 1.0);
        // m_lights[1].specular_exp = 4;
    }

    Scene::~Scene() {
        deleteBuffers();
    }

    void Scene::setViewport(unsigned int pixel_width, unsigned int pixel_height) {
        m_vp_width = pixel_width;
        m_vp_height = pixel_height;

        m_projection = glm::perspective<float>(
            20,
            (float)m_vp_width / (float)m_vp_height,
            0.1f, 100);
    }

    void Scene::addMesh(Mesh::wptr_type mesh) {
        m_meshes.push_back(mesh);
        Mesh::sptr_type smesh = mesh.lock();

        if (smesh) {
            Program::sptr_type sprogram = smesh->program().lock();
            if (sprogram) {
                GLuint progid = sprogram->getProgramId();
                const IndexMap &program_blocks = sprogram->getUniformBlocks();
                for (auto buffer_binding : m_uniform_buffer_bindings) {
                    auto block_elem = program_blocks.find(buffer_binding.first);
                    if (block_elem != program_blocks.end()) {
                        glUniformBlockBinding(progid, block_elem->second, buffer_binding.second);
                    }
                }
            }
        }
    }

    Mesh::wptr_type Scene::removeMesh(Mesh::wptr_type mesh) {
        Mesh::wptr_type rv;

        for (auto mi = m_meshes.begin(); mi != m_meshes.end(); ++mi) {
            if (!mi->owner_before(mesh) && !mesh.owner_before(*mi)) {
                rv = *mi;
                m_meshes.erase(mi);
                break;
            }
        }

        return rv;
    }

    void Scene::createBuffers() {
        GLuint bufids[2];

        deleteBuffers();
        glGenBuffers(2, bufids);

        const ViewAndProjectionBlock &vp_block = ViewAndProjectionBlock::getDescriptor();
        const LightListBlock &light_block = LightListBlock::getDescriptor();

        m_view_projection_uniform_buffer = bufids[0];
        glBindBuffer(GL_UNIFORM_BUFFER, m_view_projection_uniform_buffer);
        glBufferData(GL_UNIFORM_BUFFER, vp_block.getDataSize(), nullptr, GL_DYNAMIC_DRAW);

        m_light_uniform_buffer = bufids[1];
        glBindBuffer(GL_UNIFORM_BUFFER, m_light_uniform_buffer);
        glBufferData(GL_UNIFORM_BUFFER, light_block.getDataSize(), nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void Scene::updateBuffers() {
        const ViewAndProjectionBlock &vp_block = ViewAndProjectionBlock::getDescriptor();
        const LightListBlock &light_block = LightListBlock::getDescriptor();
        GLubyte *buffer = nullptr;

        glm::mat4x4 view = m_camera.viewTransformation();
        glm::mat4x4 view_inv = glm::inverse(view);

        glBindBuffer(GL_UNIFORM_BUFFER, m_view_projection_uniform_buffer);
        buffer = (GLubyte*)glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE);
        for (auto off_ptr : vp_block.getOffsets()) {
            switch (off_ptr.first) {
            case ViewAndProjectionBlock::field_name::VIEW:
                // glBufferSubData(GL_UNIFORM_BUFFER, off_ptr.second, sizeof(view), glm::value_ptr(view));
                *(glm::mat4x4*)(buffer + off_ptr.second) = view;
                break;
            case ViewAndProjectionBlock::field_name::VIEW_INV:
                // glBufferSubData(GL_UNIFORM_BUFFER, off_ptr.second, sizeof(view_inv), glm::value_ptr(view_inv));
                *(glm::mat4x4*)(buffer + off_ptr.second) = view_inv;
                break;
            case ViewAndProjectionBlock::field_name::PROJECTION:
                // glBufferSubData(GL_UNIFORM_BUFFER, off_ptr.second, sizeof(m_projection), glm::value_ptr(m_projection));
                *(glm::mat4x4*)(buffer + off_ptr.second) = m_projection;
                break;
            }
        }
        glUnmapBuffer(GL_UNIFORM_BUFFER);

        glBindBuffer(GL_UNIFORM_BUFFER, m_light_uniform_buffer);
        buffer = (GLubyte*)glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE);
        for (auto i = 0; i < MAX_LIGHTS; ++i) {
            GLuint enabled = m_lights[i].enabled ? GL_TRUE : GL_FALSE;

            if (m_lights[i].enabled) {
                for (auto off_ptr : light_block.getOffsets()[i]) {
                    switch (off_ptr.first) {
                    case LightListBlock::field_name::ENABLED:
                        // glBufferSubData(GL_UNIFORM_BUFFER, off_ptr.second, sizeof(GLuint), &enabled);
                        *(GLuint*)(buffer + off_ptr.second) = enabled;
                        break;
                    case LightListBlock::field_name::POSITION:
                        // glBufferSubData(GL_UNIFORM_BUFFER, off_ptr.second, sizeof(m_lights[i].position), glm::value_ptr(m_lights[i].position));
                        *(glm::vec3*)(buffer + off_ptr.second) = m_lights[i].position;
                        break;
                    case LightListBlock::field_name::COLOR:
                        // glBufferSubData(GL_UNIFORM_BUFFER, off_ptr.second, sizeof(m_lights[i].color), glm::value_ptr(m_lights[i].color));
                        *(glm::vec4*)(buffer + off_ptr.second) = m_lights[i].color;
                        break;
                    case LightListBlock::field_name::SPECULAR_EXP:
                        // glBufferSubData(GL_UNIFORM_BUFFER, off_ptr.second, sizeof(m_lights[i].specular_exp), &m_lights[i].specular_exp);
                        *(GLint*)(buffer + off_ptr.second) = m_lights[i].specular_exp;
                        break;
                    }
                }
            } else {
                auto off_ptr = light_block.getOffsets()[i].find(LightListBlock::field_name::ENABLED);
                // glBufferSubData(GL_UNIFORM_BUFFER, off_ptr->second, sizeof(GLuint), &enabled);
                *(GLuint*)(buffer + off_ptr->second) = enabled;
            }
        }
        glUnmapBuffer(GL_UNIFORM_BUFFER);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void Scene::bindBuffers() {
        glBindBufferBase(
            GL_UNIFORM_BUFFER,
            m_uniform_buffer_bindings["view_and_projection"],
            m_view_projection_uniform_buffer);

        glBindBufferBase(
            GL_UNIFORM_BUFFER,
            m_uniform_buffer_bindings["light_list"],
            m_light_uniform_buffer);
    }

    void Scene::unbindBuffers() {
        glBindBufferBase(GL_UNIFORM_BUFFER, m_uniform_buffer_bindings["view_and_projection"], 0);
        glBindBufferBase(GL_UNIFORM_BUFFER, m_uniform_buffer_bindings["light_list"], 0);
    }

    void Scene::deleteBuffers() {
        if (glIsBuffer(m_view_projection_uniform_buffer)) {
            glDeleteBuffers(1, &m_view_projection_uniform_buffer);
        }

        if (glIsBuffer(m_light_uniform_buffer)) {
            glDeleteBuffers(1, &m_light_uniform_buffer);
        }

        m_view_projection_uniform_buffer = 0;
        m_light_uniform_buffer = 0;
    }

    void Scene::render() {
        updateBuffers();
        bindBuffers();

        for (auto wm : m_meshes) {
            if (auto sm = wm.lock()) {
                sm->render();
            }
        }

        unbindBuffers();
    }
};
