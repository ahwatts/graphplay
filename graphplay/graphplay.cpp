// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <GL/glew.h>
#include <GL/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include "Geometry.h"
#include "Material.h"
#include "graphplay.h"

void initGLFW(int width, int height, const char *title, GLFWwindow **window);
void initGLEW();
void bailout(const std::string &msg);
void display(int width, int height, graphplay::Geometry &geo, graphplay::Material &mat);

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
    geo.generateBuffers();
    mat.createProgram();

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);

    glfwSetKeyCallback(window, keypress);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        display(width, height, geo, mat);
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

void display(int width, int height, graphplay::Geometry &geo, graphplay::Material &mat) {
    auto projection = glm::perspective<float>(
        90,
        (float)width / (float)height, 
        0.1f, 100);

    auto model_view = glm::lookAt<float>(
        glm::vec3(  0,  0,  4),
        glm::vec3(  0,  0,  0),
        glm::vec3(  0,  1,  0));

    glUseProgram(mat.getProgram());

    GLuint position_loc = mat.getPositionLocation();
    GLuint color_loc = mat.getColorLocation();
    GLuint proj_loc = mat.getProjectionLocation();
    GLuint mv_loc = mat.getModelViewLocation();
    unsigned int stride = geo.getStride()*sizeof(float);

    glBindBuffer(GL_ARRAY_BUFFER, geo.getArrayBuffer());
    glEnableVertexAttribArray(position_loc);
    glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, stride, geo.getPositionOffset());
    glEnableVertexAttribArray(color_loc);
    glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, stride, geo.getColorOffset());

    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(mv_loc, 1, GL_FALSE, glm::value_ptr(model_view));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geo.getElementBuffer());
    glDrawElements(GL_TRIANGLES, geo.getNumVertices(), GL_UNSIGNED_INT, 0);
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
