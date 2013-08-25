// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <ctime>
#include <GL/glew.h>
#include <GL/glfw3.h>

#include <string>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Geometry.h"
#include "Material.h"
#include "Mesh.h"
#include "Scene.h"

void initGLFW(int width, int height, const char *title, GLFWwindow **window);
void initGLEW();
void bailout(const std::string &msg);

#if GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR == 0 && GLFW_VERSION_REVISION == 0
void keypress(GLFWwindow *wnd, int key, int action);
#else
void keypress(GLFWwindow *wnd, int key, int scancode, int action, int mods);
#endif

int main(int argc, char **argv) {
    int width = 800, height = 600;
    GLFWwindow *window;

    initGLFW(width, height, "Graphplay", &window);
    initGLEW();

    graphplay::sp_Geometry octo_geo(new graphplay::OctohedronGeometry());
    graphplay::sp_Material gour_mat(new graphplay::GouraudMaterial());
    octo_geo->generateBuffers();
    gour_mat->createProgram();

    graphplay::sp_Mesh octo(new graphplay::Mesh(octo_geo, gour_mat));

    graphplay::Scene scene(width, height);
    scene.addMesh(octo);

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);

    glfwSetKeyCallback(window, keypress);

    std::clock_t pctime = std::clock(), ctime;
    glm::mat4x4 mv;
    glm::vec3 yhat = glm::vec3(0.0, 1.0, 0.0);
    float rotation = 0.0;

    while (!glfwWindowShouldClose(window)) {
        ctime = std::clock();
        float dtime = (float)(ctime - pctime) / (float)CLOCKS_PER_SEC;
        pctime = ctime;
        rotation += 90.0 * dtime;
        if (rotation >= 360.0) { rotation -= 360.0; }

        mv = glm::mat4x4();
        mv = glm::rotate(mv, rotation, yhat);
        octo->setTransform(mv);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        scene.render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void initGLFW(int width, int height, const char *title, GLFWwindow **window) {
    if (!glfwInit()) {
        bailout("Could not initialize GLFW!");
    }

    *window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!*window) {
        bailout("Could not create window!");
    }

    glfwMakeContextCurrent(*window);
}

void initGLEW() {
    GLenum glew_err = glewInit();
    if (glew_err != GLEW_OK) {
        std::ostringstream msg;
        msg << "Could not initialize GLEW: " << glewGetErrorString(glew_err);
        bailout(msg.str());
    }
}

void bailout(const std::string &msg) {
    std::cerr << msg << std::endl;
    glfwTerminate();
    std::exit(1);
}

#if GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR == 0 && GLFW_VERSION_REVISION == 0
void keypress(GLFWwindow *wnd, int key, int action) {
#else
void keypress(GLFWwindow *wnd, int key, int scancode, int action, int mods) {
#endif
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(wnd, true);
    }
}
