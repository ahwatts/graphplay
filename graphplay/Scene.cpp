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
          m_meshes(),
          m_view_projection_uniform_buffer(0),
          m_uniform_buffer_bindings()
    {
        m_uniform_buffer_bindings["view_and_projection"] = 0;
        setViewport(vp_width, vp_height);
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
        deleteBuffers();
        glGenBuffers(1, &m_view_projection_uniform_buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, m_view_projection_uniform_buffer);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(ViewAndProjectionBlock), nullptr, GL_DYNAMIC_DRAW);
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
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void Scene::bindBuffers() {
        glBindBufferBase(
            GL_UNIFORM_BUFFER,
            m_uniform_buffer_bindings["view_and_projection"],
            m_view_projection_uniform_buffer);
    }

    void Scene::unbindBuffers() {
        glBindBufferBase(GL_UNIFORM_BUFFER, m_uniform_buffer_bindings["view_and_projection"], 0);
    }

    void Scene::deleteBuffers() {
        if (glIsBuffer(m_view_projection_uniform_buffer)) {
            glDeleteBuffers(1, &m_view_projection_uniform_buffer);
        }
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
