// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <GL/glew.h>
#include <GL/glfw3.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include "Collada.h"
#include "Geometry.h"
#include "Material.h"

void bailout(const std::string &msg);

#if GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR == 0 && GLFW_VERSION_REVISION == 0
void keypress(GLFWwindow *wnd, int key, int action);
#else
void keypress(GLFWwindow *wnd, int key, int scancode, int action, int mods);
#endif

int main(int argc, char **argv) {
    int width = 800, height = 600;

    if (!glfwInit()) { bailout("Could not initialize GLFW!"); }
    GLFWwindow *window = glfwCreateWindow(width, height, "Graphplay", NULL, NULL);
    if (!window) { bailout("Could not create window!"); }
    glfwMakeContextCurrent(window);

    GLenum glew_err = glewInit();
    if (glew_err != GLEW_OK) {
        std::ostringstream msg;
        msg << "Could not initialize GLEW: " << glewGetErrorString(glew_err);
        bailout(msg.str());
    }

    std::vector<collada::MeshGeometry> geos;
    collada::loadGeometriesFromFile(geos, "chair.dae");
    graphplay::Geometry geo(geos[0]);
    graphplay::GouraudMaterial mat;
    mat.createProgram();

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);

    glfwSetKeyCallback(window, keypress);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
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
