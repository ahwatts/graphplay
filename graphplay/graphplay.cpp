// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <GL/glew.h>
#include <GL/glfw3.h>
#include <string>
#include <initializer_list>
#include <iostream>
#include <vector>

#include "Collada.h"
#include "Geometry.h"
#include "Material.h"

void bailout(std::initializer_list<std::string> msgs);
void keypress(GLFWwindow *wnd, int key, int scancode);

int main(int argc, char **argv) {
    int width = 800, height = 600;

    if (!glfwInit()) { bailout({ "Could not initialize GLFW!" }); }
    GLFWwindow *window = glfwCreateWindow(width, height, "Graphplay", NULL, NULL);
    if (!window) { bailout({ "Could not create window!" }); }
    glfwMakeContextCurrent(window);

    GLenum glew_err = glewInit();
    if (glew_err != GLEW_OK) {
        bailout({ "Could not initialize GLEW: ", (const char *)glewGetErrorString(glew_err) });
    }

    std::vector<collada::MeshGeometry> geos;
    collada::loadGeometriesFromFile(geos, "chair.dae");
    graphplay::Geometry geo(geos[0]);
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

void bailout(std::initializer_list<std::string> msgs) {
    for (auto m : msgs) std::cerr << m;
    std::cerr << std::endl;
    glfwTerminate();
    std::exit(1);
}

void keypress(GLFWwindow *wnd, int key, int scancode) {
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(wnd, true);
    }
}
