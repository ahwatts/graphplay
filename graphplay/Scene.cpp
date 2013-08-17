// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Scene.h"

namespace graphplay {
    Scene::Scene(unsigned int vp_width, unsigned int vp_height)
        : m_perspective(),
          m_model_view(),
          m_vp_width(vp_width),
          m_vp_height(vp_height),
          m_meshes() { }

    Scene::~Scene(void) { }

    void Scene::setViewport(unsigned int width, unsigned int height) {
        m_vp_width = width;
        m_vp_height = height;
    }

    void Scene::addMesh(wp_Mesh mesh) {
        m_meshes.push_back(mesh);
    }

    wp_Mesh Scene::removeMesh(wp_Mesh &mesh) {
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

        m_model_view = glm::lookAt<float>(
            glm::vec3(  0,  0,  3),
            glm::vec3(  0,  0,  0),
            glm::vec3(  0,  1,  0));

        for (auto wm : m_meshes) {
            if (auto sm = wm.lock()) {
                sm->render(m_perspective, m_model_view);
            }
        }
    }
};
