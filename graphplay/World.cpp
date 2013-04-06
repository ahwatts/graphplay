#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/ext.hpp>
#include "World.h"

World::World(unsigned int vp_width, unsigned int vp_height)
    : perspective(),
      model_view(),
      vp_width(vp_width),
      vp_height(vp_height)
{ }

World::~World(void) { }

void World::update(float dt) {
    // nothing to do here...
}

void World::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, vp_width, vp_height);

    perspective = glm::perspective<float>(
        glm::quarter_pi<float>(),
        (float)vp_width / (float)vp_height, 
        -1, 1000);

    model_view = glm::lookAt<float>(
        glm::vec3(0, 0, 10),
        glm::vec3(0, 0,  0),
        glm::vec3(0, 1,  0));

    glutSwapBuffers();
}
