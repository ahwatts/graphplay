// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <GL/glew.h>
#include <GL/glfw3.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include "Geometry.h"
#include "Material.h"

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

    graphplay::OctohedronGeometry geo;
    graphplay::GouraudMaterial mat;

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
