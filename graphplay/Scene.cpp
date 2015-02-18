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
        : m_vp_width(0),
          m_vp_height(0),
          m_camera(),
          m_projection(),
          m_lights(),
          m_meshes(),
          m_view_projection_uniform_buffer(0),
          m_uniform_buffer_bindings()
    {
        m_uniform_buffer_bindings["view_and_projection"] = 0;
        m_uniform_buffer_bindings["light_list"] = 1;
        setViewport(vp_width, vp_height);

        for (auto i = 0; i < MAX_LIGHTS; ++i) {
            m_lights.lights[i].enabled = 0;
        }

        LightPropertiesBlock &light = m_lights.lights[0];
        glm::vec4 color_vec(1.0, 1.0, 1.0, 1.0);
        glm::vec3 position_vec(0.0, 10.0, 0.0);
        float *color = glm::value_ptr(color_vec), *position = glm::value_ptr(position_vec);
        light.enabled = 1;
        std::copy(color, color + 4, light.color);
        std::copy(position, position + 3, light.position);
        light.specular_exp = 4;

        createBuffers();
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
            Program::sptr_type sprogram = smesh->getProgram().lock();
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

        m_view_projection_uniform_buffer = bufids[0];
        glBindBuffer(GL_UNIFORM_BUFFER, m_view_projection_uniform_buffer);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(ViewAndProjectionBlock), nullptr, GL_DYNAMIC_DRAW);

        m_light_uniform_buffer = bufids[1];
        glBindBuffer(GL_UNIFORM_BUFFER, m_light_uniform_buffer);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(LightListBlock), nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void Scene::updateBuffers() {
        glm::mat4x4 view = m_camera.getViewTransform();
        glm::mat4x4 view_inv = glm::inverse(view);
        ViewAndProjectionBlock block;
        float *view_ptr = glm::value_ptr(view);
        float *view_inv_ptr = glm::value_ptr(view_inv);
        float *proj_ptr = glm::value_ptr(m_projection);

        std::copy(view_ptr, view_ptr + 16, block.view);
        std::copy(view_inv_ptr, view_inv_ptr + 16, block.view_inv);
        std::copy(proj_ptr, proj_ptr + 16, block.projection);

        glBindBuffer(GL_UNIFORM_BUFFER, m_view_projection_uniform_buffer);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ViewAndProjectionBlock), &block);

        glBindBuffer(GL_UNIFORM_BUFFER, m_light_uniform_buffer);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightListBlock), &m_lights);

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
