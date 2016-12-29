// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "graphplay.h"
#include "config.h"

#include <chrono>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <thread>

#include <glm/gtx/io.hpp>

#include "opengl.h"

#include "fzx/PhysicsSystem.h"
#include "gfx/Scene.h"
#include "Driver.h"

void init_glfw(int width, int height, const char *title, GLFWwindow **window);
void init_glad();
void handle_glfw_error(int code, const char *desc);
void bailout(const std::string &msg);

void keypress(GLFWwindow *wnd, int key, int scancode, int action, int mods);
void mouse_click(GLFWwindow *wnd, int button, int action, int mods);
void mouse_scroll(GLFWwindow *wnd, double xoffset, double yoffset);
void mouse_move(GLFWwindow *wnd, double xpos, double ypos);

// using namespace graphplay;
// using namespace boost::filesystem;
// using namespace std::chrono;

// const duration<float> FRAME_RATE(1.0f / 60.0f);
// const float TIME_STEP = FRAME_RATE.count() / 5;

// gfx::Scene SCENE(1024, 768);
// fzx::Body::sptr_type ORIGIN = std::make_shared<fzx::Body>();
double MOUSE_X = 0.0, MOUSE_Y = 0.0;
bool ROTATING = false;
// std::random_device RANDOM_DEV;
// std::default_random_engine RANDOM_ENG(RANDOM_DEV());

int main(int argc, char **argv) {
    int pixel_width = 1024, pixel_height = 768;
    GLFWwindow *window = nullptr;

    init_glfw(pixel_width, pixel_height, "Graphplay", &window);
    init_glad();

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "OpenGL renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL vendor: " << glGetString(GL_VENDOR) << std::endl;

    glfwGetFramebufferSize(window, &pixel_width, &pixel_height);
    glfwGetCursorPos(window, &MOUSE_X, &MOUSE_Y);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glViewport(0, 0, pixel_width, pixel_height);

    glfwSetKeyCallback(window, keypress);
    glfwSetCursorPosCallback(window, mouse_move);
    glfwSetMouseButtonCallback(window, mouse_click);
    glfwSetScrollCallback(window, mouse_scroll);

    graphplay::drive(window);

    glfwTerminate();
    return 0;
}

void init_glfw(int width, int height, const char *title, GLFWwindow **window) {
    glfwSetErrorCallback(handle_glfw_error);
    if (!glfwInit()) {
        bailout("Could not initialize GLFW!");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    *window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!*window) {
        bailout("Could not create window!");
    }

    glfwMakeContextCurrent(*window);
}

void init_glad() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::ostringstream msg;
        msg << "Could not initialize OpenGL context." << std::endl;
        bailout(msg.str());
    }
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

void keypress(GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    if (action == 1) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(wnd, true);
            break;
        default:
            std::cout << "key: " << key
                      << " scancode: " << scancode
                      << " action: " << action
                      << " mods: " << mods
                      << std::endl;
        }
    }
}

void mouse_click(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            ROTATING = true;
            glfwGetCursorPos(window, &MOUSE_X, &MOUSE_Y);
        } else if (action == GLFW_RELEASE) {
            ROTATING = false;
        }
    }
}

void mouse_scroll(GLFWwindow *wnd, double xoffset, double yoffset) {
    // SCENE.getCamera().zoom(yoffset);
}

void mouse_move(GLFWwindow *wnd, double xpos, double ypos) {
    // if (ROTATING) {
    //     double theta = -1 * 2 * M_PI * ((xpos - MOUSE_X) / SCENE.getViewportWidth());
    //     double phi = -1 * M_PI * ((ypos - MOUSE_Y) / SCENE.getViewportHeight());

    //     SCENE.getCamera().rotate(theta, phi);
    //     MOUSE_X = xpos;
    //     MOUSE_Y = ypos;
    // }
}
