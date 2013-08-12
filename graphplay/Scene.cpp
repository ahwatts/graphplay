// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/constants.hpp>
#include "Scene.h"
#include "SphereBody.h"
#include "graphplay.h"

namespace graphplay {
    Scene::Scene(unsigned int vp_width, unsigned int vp_height)
        : m_perspective(),
          m_model_view(),
          m_vp_width(vp_width),
          m_vp_height(vp_height),
          m_bodies() {
        /*SphereBody *sphere = new SphereBody(*g_shader);
        sphere->m_ang_vel = 20.0f;
        sphere->mw_pos = glm::vec3(1, 0, 0);
        m_bodies.push_back(sphere);*/
    }

    Scene::~Scene(void) {
        for (unsigned int i = 0; i < m_bodies.size(); ++i) {
            delete m_bodies[i];
        }
    }

    void Scene::setViewport(unsigned int width, unsigned int height) {
        m_vp_width = width;
        m_vp_height = height;
    }

    void Scene::update(float dt) {
        for (unsigned int i = 0; i < m_bodies.size(); ++i) {
            m_bodies[i]->update(dt);
        }
    }

    void Scene::render() {
        m_perspective = glm::perspective<float>(
            45,
            (float)m_vp_width / (float)m_vp_height, 
            0.1f, 100);

        m_model_view = glm::lookAt<float>(
            glm::vec3(  0,  0,  5),
            glm::vec3(  0,  0,  0),
            glm::vec3(  0,  1,  0));

        for (unsigned int i = 0; i < m_bodies.size(); ++i) {
            m_bodies[i]->render(m_perspective, m_model_view, 0);
        }
    }
};
