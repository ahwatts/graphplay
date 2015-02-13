// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "graphplay.h"
#include "config.h"

#ifdef MSVC
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <string>
#include <iostream>
#include <sstream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

#ifndef MSVC
#include <sys/time.h>
#endif

#include "Geometry.h"
#include "Material.h"
#include "Mesh.h"
#include "Scene.h"
#include "Shader.h"
#include "opengl.h"

void initGLFW(int width, int height, const char *title, GLFWwindow **window);
void initGLEW();
void handle_glfw_error(int code, const char *desc);
void bailout(const std::string &msg);
void keypress(GLFWwindow *wnd, int key, int scancode, int action, int mods);

// typedef enum { OCTOHEDRON, CUBE } view_state_t;
// static view_state_t view_state = OCTOHEDRON, new_view_state = OCTOHEDRON;

// typedef enum { GOURAUD, LAMBERT, PHONG } lighting_state_t;
// static lighting_state_t light_state = PHONG, new_light_state = PHONG;

using namespace graphplay;

int main(int argc, char **argv) {
    int screen_width = 800, screen_height = 600;
    int pixel_width = screen_width, pixel_height = screen_height;
    GLFWwindow *window;

    initGLFW(screen_width, screen_height, "Graphplay", &window);
    initGLEW();
    glfwGetFramebufferSize(window, &pixel_width, &pixel_height);

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "OpenGL renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL vendor: " << glGetString(GL_VENDOR) << std::endl;

    Geometry<PCNVertex>::sptr_type octo_geo = makeOctohedronGeometry();
    // Geometry<PCNVertex>::sptr_type sphere_geo = makeSphereGeometry();

    Shader::sptr_type unlit_vertex_shader = std::make_shared<Shader>(GL_VERTEX_SHADER, Shader::unlit_vertex_shader_source);
    Shader::sptr_type unlit_fragment_shader = std::make_shared<Shader>(GL_FRAGMENT_SHADER, Shader::unlit_fragment_shader_source);
    Program::sptr_type unlit_program = std::make_shared<Program>(unlit_vertex_shader, unlit_fragment_shader);

    Mesh::sptr_type octo = std::make_shared<Mesh>(octo_geo, unlit_program);
    // Mesh::sptr_type sphere = std::make_shared<Mesh>(sphere_geo, unlit_shader);

    Scene scene(pixel_width, pixel_height);
    scene.addMesh(octo);
    // scene.addMesh(sphere);

    Camera &camera = scene.getCamera();
    camera.setLocation(glm::vec3(0, 0, 3));
    camera.setDirection(glm::vec3(0, 0, 0));
    camera.setUpDirection(glm::vec3(0, 1, 0));

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, pixel_width, pixel_height);

    glfwSetKeyCallback(window, keypress);

    glm::mat4x4 mv;
    glm::vec3 yhat = glm::vec3(0, 1, 0);
    glm::vec3 xhat = glm::vec3(1, 0, 0);
    // glm::vec3 offset = glm::vec3(-1, -1, -1);
    // glm::vec3 scale = glm::vec3(2, 2, 2);
    double yrot = 0, xrot = 0;
    
#ifdef MSVC
    LARGE_INTEGER time, ptime, frequency, tick_delta;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&ptime);
#else
    struct timeval tod, ptod;
    gettimeofday(&ptod, NULL);
#endif

    while (!glfwWindowShouldClose(window)) {

        // Calculate the time delta.
#ifdef MSVC
        QueryPerformanceCounter(&time);
        tick_delta.QuadPart = time.QuadPart - ptime.QuadPart;
        auto delta = tick_delta.QuadPart * 1000000 / frequency.QuadPart; // delta is now in usec.
        ptime = time;
#else
        gettimeofday(&tod, NULL);
        auto delta = (tod.tv_sec * 1000000 + tod.tv_usec - ptod.tv_sec * 1000000 - ptod.tv_usec); // delta in usec.
        ptod = tod;
#endif

        double dtime = delta / 1e6;
        // std::cout << "delta = " << delta << " dtime = " << dtime << std::endl;

        // Update the rotation based on the time delta.
        yrot += M_PI / 20.0 * dtime;
        if (yrot >= 2*M_PI) { yrot -= 2*M_PI; }

        xrot += M_PI / 60.0 * dtime;
        if (xrot >= 2*M_PI) { xrot -= 2*M_PI; }

        // Handle input.
        // if (new_light_state != light_state) {
        //     light_state = new_light_state;
        //     switch (new_light_state) {
        //     case GOURAUD: sphere->setMaterial(gour_mat); break;
        //     case LAMBERT: sphere->setMaterial(lamb_mat); break;
        //     case PHONG: sphere->setMaterial(phong_mat); break;
        //     }
        // }

        // Create the modelview matrix.
        mv = glm::mat4x4();
        mv = glm::rotate(mv, (float)yrot, yhat);
        mv = glm::rotate(mv, (float)xrot, xhat);

        // Make the meshes use the modelview matrix.
        octo->setTransform(mv);
        // sphere->setTransform(mv);

        // render.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        scene.render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void initGLFW(int width, int height, const char *title, GLFWwindow **window) {
    glfwSetErrorCallback(handle_glfw_error);
    if (!glfwInit()) {
        bailout("Could not initialize GLFW!");
    }

#ifdef __APPLE_CC__
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    *window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!*window) {
        bailout("Could not create window!");
    }

    glfwMakeContextCurrent(*window);
}

void initGLEW() {
#ifndef __APPLE_CC__
    GLenum glew_err = glewInit();
    if (glew_err != GLEW_OK) {
        std::ostringstream msg;
        msg << "Could not initialize GLEW: " << glewGetErrorString(glew_err);
        bailout(msg.str());
    }
#endif
}

void handle_glfw_error(int code, const char *desc) {
    std::cerr << "GLFW Error Code " << code << std::endl
              << desc << std::endl;
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
        //case 'S':
        //case 's':
        //    if (vew_state == OCTOHEDRON) {
        //        view_state = CUBE;
        //    }
        //    else if (view_state == CUBE) {
        //        view_state = OCTOHEDRON;
        //    }
        //    break;
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
