// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "config.h"

#ifdef MSVC
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <sys/time.h>
#endif

#include <GL/glew.h>
#include GLFW_HEADER

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

// typedef enum { OCTOHEDRON, CUBE } view_state_t;
// static view_state_t view_state = OCTOHEDRON, new_view_state = OCTOHEDRON;

typedef enum { GOURAUD, LAMBERT, PHONG } lighting_state_t;
// static lighting_state_t light_state = LAMBERT, new_light_state = LAMBERT;

int main(int argc, char **argv) {
    int width = 800, height = 600;
    GLFWwindow *window;

    initGLFW(width, height, "Graphplay", &window);
    initGLEW();

    graphplay::sp_Geometry octo_geo(new graphplay::OctohedronGeometry());
    // graphplay::sp_Geometry octo_normals_geo(new graphplay::NormalGeometry(*octo_geo));
    // graphplay::sp_Geometry cube_geo(new graphplay::CubeGeometry());
    octo_geo->generateBuffers();
    // octo_normals_geo->generateBuffers();
    // cube_geo->generateBuffers();

    // graphplay::sp_Material gour_mat(new graphplay::GouraudMaterial());
    // graphplay::sp_Material lamb_mat(new graphplay::LambertMaterial());
    // graphplay::sp_Material phong_mat(new graphplay::PhongMaterial());
    // gour_mat->createProgram();
    // lamb_mat->createProgram();
    // phong_mat->createProgram();

    // graphplay::sp_Mesh octo(new graphplay::Mesh(octo_geo, lamb_mat));
    // graphplay::sp_Mesh octo_normals(new graphplay::Mesh(octo_normals_geo, gour_mat));
    // graphplay::sp_Mesh cube(new graphplay::Mesh(cube_geo, lamb_mat));

    graphplay::sp_Mesh octo(new graphplay::DebugMesh(octo_geo));

    graphplay::Scene scene(width, height);
    scene.addMesh(octo);
    // scene.addMesh(octo_normals);
    // scene.addMesh(cube);

    graphplay::Camera &camera = scene.getCamera();
    camera.setLocation(glm::vec3(0, 0, 3));
    camera.setDirection(glm::vec3(0, 0, 0));
    camera.setUpDirection(glm::vec3(0, 1, 0));

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);

    glfwSetKeyCallback(window, keypress);

    glm::mat4x4 mv;
    glm::vec3 yhat = glm::vec3(0, 1, 0);
    glm::vec3 xhat = glm::vec3(1, 0, 0);
    // glm::vec3 offset = glm::vec3(-1, -1, -1);
    // glm::vec3 scale = glm::vec3(2, 2, 2);
    float yrot = 0, xrot = 0;
    
#ifdef MSVC
    SYSTEMTIME stime, pstime;
    GetSystemTime(&pstime);
#else
    struct timeval tod, ptod;
    gettimeofday(&ptod, NULL);
#endif

    while (!glfwWindowShouldClose(window)) {

        // Calculate the time delta.
#ifdef MSVC
        GetSystemTime(&stime);
        int msec = stime.wMilliseconds - pstime.wMilliseconds;
        if (msec < 0) msec = 1000 + msec;
        int delta = msec * 1000;
        pstime = stime;
#else
        gettimeofday(&tod, NULL);
        auto delta = tod.tv_sec * 1000000 + tod.tv_usec - ptod.tv_sec * 1000000 - ptod.tv_usec;
        ptod = tod;
#endif

        float dtime = delta / 1e6f;

        // Update the rotation based on the time delta.
        yrot += 90.0f * dtime;
        if (yrot >= 360.0) { yrot -= 360.0; }

        xrot += 45.0f * dtime;
        if (xrot >= 360.0) { xrot -= 360.0; }

        // Handle input.
        //if (new_light_state != light_state) {
        //    light_state = new_light_state;
        //    switch (new_light_state) {
        //    case GOURAUD: octo->setMaterial(gour_mat); break;
        //    case LAMBERT: octo->setMaterial(lamb_mat); break;
        //    case PHONG: octo->setMaterial(phong_mat); break;
        //    }
        //}

        // Create the modelview matrix.
        mv = glm::mat4x4();
        mv = glm::rotate(mv, yrot, yhat);
        mv = glm::rotate(mv, xrot, xhat);

        // Make the meshes use the modelview matrix.
        octo->setTransform(mv);
        // octo_normals->setTransform(mv);

        // render.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        scene.render();
        glfwSwapBuffers(window);
        glfwPollEvents();

        ((graphplay::DebugMesh *)octo.get())->printTransformFeedback();
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
    if (action == 1) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(wnd, true);
            break;
        // case 'S':
        // case 's':
        //     if (vew_state == OCTOHEDRON) {
        //         view_state = CUBE;
        //     } else if (view_state == CUBE) {
        //         view_state = OCTOHEDRON;
        //     }
        //     break;
        // case 'Q':
        // case 'q':
        //     new_light_state = GOURAUD;
        //     break;
        // case 'W':
        // case 'w':
        //     new_light_state = LAMBERT;
        //     break;
        // case 'E':
        // case 'e':
        //     new_light_state = PHONG;
        //     break;
        default:
            std::cout << "key: " << key
                      << " action: " << action
                      << std::endl;
        }
    }
}
