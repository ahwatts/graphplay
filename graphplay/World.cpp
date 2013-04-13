#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/constants.hpp>
#include "World.h"
#include "SphereBody.h"
#include "graphplay.h"

World::World(unsigned int vp_width, unsigned int vp_height)
    : m_perspective(),
      m_model_view(),
      m_vp_width(vp_width),
      m_vp_height(vp_height),
      m_bodies()
{
    SphereBody *sphere = new SphereBody(g_shader->getProgram());
    m_bodies.push_back(sphere);
}

World::~World(void)
{
    for (unsigned int i = 0; i < m_bodies.size(); ++i) {
        delete m_bodies[i];
    }
}

void World::update(float dt)
{
    // nothing to do here...
}

void World::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, m_vp_width, m_vp_height);

    m_perspective = glm::perspective<float>(
        glm::quarter_pi<float>(),
        (float)m_vp_width / (float)m_vp_height, 
        -1, 1000);

    m_model_view = glm::lookAt<float>(
        glm::vec3(0, 0, 10),
        glm::vec3(0, 0,  0),
        glm::vec3(0, 1,  0));

    glutSwapBuffers();
}
