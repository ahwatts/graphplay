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

#include "Camera.h"
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

    graphplay::sp_Geometry octo_geo(new graphplay::CubeGeometry());
    graphplay::sp_Material gour_mat(new graphplay::LambertMaterial());
    octo_geo->generateBuffers();
    gour_mat->createProgram();

    graphplay::sp_Mesh octo(new graphplay::Mesh(octo_geo, gour_mat));

    graphplay::Scene scene(width, height);
    scene.addMesh(octo);

    graphplay::Camera &camera = scene.getCamera();
    camera.setLocation(glm::vec3(0, 0, 3));
    camera.setDirection(glm::vec3(0, 0, 0));
    camera.setUpDirection(glm::vec3(0, 1, 0));

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);

    glfwSetKeyCallback(window, keypress);

    std::clock_t pctime = std::clock(), ctime;
    glm::mat4x4 mv;
    glm::vec3 yhat = glm::vec3(0, 1, 0);
    glm::vec3 xhat = glm::vec3(1, 0, 0);
    glm::vec3 offset = glm::vec3(-1, -1, -1);
    glm::vec3 scale = glm::vec3(2, 2, 2);
    float yrot = 0, xrot = 0;

    while (!glfwWindowShouldClose(window)) {
        ctime = std::clock();
        float dtime = (float)(ctime - pctime) / (float)CLOCKS_PER_SEC;
        pctime = ctime;

        yrot += 90.0f * dtime;
        if (yrot >= 360.0) { yrot -= 360.0; }

        xrot += 45.0f * dtime;
        if (xrot >= 360.0) { xrot -= 360.0; }

        mv = glm::mat4x4();
        mv = glm::rotate(mv, yrot, yhat);
        mv = glm::rotate(mv, xrot, xhat);
        mv = glm::translate(mv, offset);
        mv = glm::scale(mv, scale);
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
