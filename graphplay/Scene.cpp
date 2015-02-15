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
          m_uniform_buffer(0)
    {
        setViewport(vp_width, vp_height);
    }

    Scene::~Scene() {
        deleteBuffer();
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
                const IndexMap &blocks = sprogram->getUniformBlocks();
                std::map<GLuint, GLuint> buffer_bindings;
                GLuint binding_index = 0;

                auto vp_elem = blocks.find("view_and_projection");
                if (vp_elem != blocks.end()) {
                    auto binding = buffer_bindings.find(m_uniform_buffer);
                    if (binding == buffer_bindings.end()) {
                        glBindBufferBase(GL_UNIFORM_BUFFER, binding_index, m_uniform_buffer);
                        buffer_bindings[m_uniform_buffer] = binding_index;
                        ++binding_index;
                        binding = buffer_bindings.find(m_uniform_buffer);
                    }

                    glUniformBlockBinding(sprogram->getProgramId(), vp_elem->second, binding->second);
                }

                // Bind other uniform buffers...
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

    void Scene::createBuffer() {
        deleteBuffer();
        glGenBuffers(1, &m_uniform_buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_buffer);
        updateBuffer();
    }

    void Scene::updateBuffer() {
        glm::mat4x4 view = m_camera.getViewTransform();
        glm::mat4x4 view_inv = glm::inverse(view);
        ViewAndProjectionBlock block;
        float *view_ptr = glm::value_ptr(view);
        float *view_inv_ptr = glm::value_ptr(view_inv);
        float *proj_ptr = glm::value_ptr(m_projection);

        std::copy(view_ptr, view_ptr + 16, block.view);
        std::copy(view_inv_ptr, view_inv_ptr + 16, block.view_inv);
        std::copy(proj_ptr, proj_ptr + 16, block.projection);

        if (!glIsBuffer(m_uniform_buffer)) createBuffer();
        glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_buffer);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(ViewAndProjectionBlock), &block, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void Scene::deleteBuffer() {
        if (glIsBuffer(m_uniform_buffer)) {
            glDeleteBuffers(1, &m_uniform_buffer);
        }
    }

    void Scene::render() {
        updateBuffer();

        for (auto wm : m_meshes) {
            if (auto sm = wm.lock()) {
                sm->render();
            }
        }
    }

    Scene::MeshIterator Scene::begin() const {
        return Scene::MeshIterator(*this, 0);
    }

    Scene::MeshIterator Scene::end() const {
        return Scene::MeshIterator(*this, m_meshes.size());
    }

    Scene::MeshIterator::MeshIterator(const Scene &scene, unsigned int init_loc)
        : m_scene(scene),
          m_loc(init_loc) { }

    bool Scene::MeshIterator::operator==(const Scene::MeshIterator &other) const {
        return !(*this != other);
    }

    bool Scene::MeshIterator::operator!=(const Scene::MeshIterator &other) const {
        return &m_scene != &other.m_scene || m_loc != other.m_loc;
    }

    Scene::value_type Scene::MeshIterator::operator*() {
        return Mesh::wptr_type(m_scene.m_meshes[m_loc]);
    }

    Scene::MeshIterator &Scene::MeshIterator::operator++() {
        ++m_loc;
        return *this;
    }

    Scene::MeshIterator Scene::MeshIterator::operator++(int) {
        Scene::MeshIterator clone(*this);
        ++m_loc;
        return clone;
    }
};
