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
        Mesh::sptr_type sm = mesh.lock();
        if (sm) {
            sm->setUpProgramUniforms(m_uniform_buffer);
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
        updateBuffer();
    }

    void Scene::updateBuffer() {
        glm::mat4x4 view = m_camera.getViewTransform();
        glm::mat4x4 view_inv = glm::inverse(view);
        ViewAndProjectionBlock block;

        std::copy(glm::value_ptr(view), glm::value_ptr(view) + 16 * sizeof(float), block.view);
        std::copy(glm::value_ptr(view_inv), glm::value_ptr(view_inv) + 16 * sizeof(float), block.view_inv);
        std::copy(glm::value_ptr(m_projection), glm::value_ptr(m_projection) + 16 * sizeof(float), block.projection);

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
        // glm::vec3 light_pos(0, 10, 10);
        // glm::vec4 light_color(1, 1, 1, 1);
        // unsigned int specular_exponent = 10;

        for (auto wm : m_meshes) {
            if (auto sm = wm.lock()) {
                Program::sptr_type program = sm->getProgram().lock();
                // if (mat) {
                //     GLuint program = mat->getProgram();
                //     GLint view_loc = mat->getViewLocation();
                //     GLint view_inv_loc = mat->getViewInverseLocation();
                //     GLint projection_loc = mat->getProjectionLocation();
                //     GLint light_pos_loc = mat->getLightPositionLocation();
                //     GLint light_color_loc = mat->getLightColorLocation();
                //     GLint specular_exp_loc = mat->getSpecularExponentLocation();

                //     glUseProgram(program);
                //     if (view_loc >= 0) glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(m_view));
                //     if (view_inv_loc >= 0) glUniformMatrix4fv(view_inv_loc, 1, GL_FALSE, glm::value_ptr(m_view_inv));
                //     if (projection_loc >= 0) glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(m_projection));
                //     if (light_pos_loc >= 0) glUniform3fv(light_pos_loc, 1, glm::value_ptr(light_pos));
                //     if (light_color_loc >= 0) glUniform4fv(light_color_loc, 1, glm::value_ptr(light_color));
                //     if (specular_exp_loc >= 0) glUniform1ui(specular_exp_loc, specular_exponent);
                // }
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
