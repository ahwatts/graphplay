// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Scene.h"

namespace graphplay {
    Scene::Scene(unsigned int vp_width, unsigned int vp_height)
        : m_perspective(),
          m_model_view(),
          m_vp_width(vp_width),
          m_vp_height(vp_height),
          m_camera(),
          m_meshes() { }

    Scene::~Scene(void) { }

    void Scene::setViewport(unsigned int width, unsigned int height) {
        m_vp_width = width;
        m_vp_height = height;
    }

    void Scene::addMesh(wp_Mesh mesh) {
        m_meshes.push_back(mesh);
    }

    wp_Mesh Scene::removeMesh(wp_Mesh mesh) {
        wp_Mesh rv;

        for (auto mi = m_meshes.begin(); mi != m_meshes.end(); ++mi) {
            if (!mi->owner_before(mesh) && !mesh.owner_before(*mi)) {
                rv = *mi;
                m_meshes.erase(mi);
                break;
            }
        }

        return rv;
    }

    void Scene::render() {
        m_perspective = glm::perspective<float>(
            90,
            (float)m_vp_width / (float)m_vp_height, 
            0.1f, 100);

        m_model_view = m_camera.getViewTransform();

        glm::vec3 light_pos(0, 0, 10);
        glm::vec4 light_color(1, 0.5, 0.25, 1);

        for (auto wm : m_meshes) {
            if (auto sm = wm.lock()) {
                sp_Material mat = sm->getMaterial().lock();
                if (mat) {
                    GLint light_pos_loc = mat->getLightPositionLocation();
                    GLint light_color_loc = mat->getLightColorLocation();
                    GLuint program = mat->getProgram();

                    if (light_pos_loc >= 0 || light_color_loc >= 0) {
                        glUseProgram(program);
                        if (light_pos_loc >= 0) glUniform3fv(light_pos_loc, 1, glm::value_ptr(light_pos));
                        if (light_color_loc >= 0) glUniform4fv(light_color_loc, 1, glm::value_ptr(light_color));
                    }
                }
                sm->render(m_perspective, m_model_view);
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
        return wp_Mesh(m_scene.m_meshes[m_loc]);
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
