// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "graphplay.h"
#include "config.h"

#include <chrono>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

#include <boost/filesystem.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Body.h"
#include "Geometry.h"
#include "Mesh.h"
#include "Scene.h"
#include "Shader.h"
#include "opengl.h"

void initGLFW(int width, int height, const char *title, GLFWwindow **window);
void initglad();
void handle_glfw_error(int code, const char *desc);
void bailout(const std::string &msg);

void keypress(GLFWwindow *wnd, int key, int scancode, int action, int mods);
void mouse_click(GLFWwindow *wnd, int button, int action, int mods);
void mouse_scroll(GLFWwindow *wnd, double xoffset, double yoffset);
void mouse_move(GLFWwindow *wnd, double xpos, double ypos);

using namespace graphplay;
using namespace boost::filesystem;
using namespace std::chrono;

Scene SCENE(1024, 768);
double MOUSE_X = 0.0, MOUSE_Y = 0.0;
bool ROTATING = false;

int main(int argc, char **argv) {
    int pixel_width = SCENE.getViewportWidth(), pixel_height = SCENE.getViewportHeight();
    GLFWwindow *window = nullptr;

    initGLFW(pixel_width, pixel_height, "Graphplay", &window);
    initglad();

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "OpenGL renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL vendor: " << glGetString(GL_VENDOR) << std::endl;

    glfwGetFramebufferSize(window, &pixel_width, &pixel_height);
    SCENE.setViewport(pixel_width, pixel_height);
    SCENE.createBuffers();

    glfwGetCursorPos(window, &MOUSE_X, &MOUSE_Y);

    // Geometry<PCNVertex>::sptr_type object_geo = makeOctohedronGeometry();
    // Geometry<PCNVertex>::sptr_type object_geo = makeSphereGeometry();

    path assets_path("assets");
    path object_path(assets_path);
    object_path /= "stanford_bunny.ply";
    // object_path /= "stanford_bunny.pcn";
    // object_path /= "stanford_armadillo.ply";
    // object_path /= "stanford_armadillo.pcn";
    Geometry<PCNVertex>::sptr_type object_geo = loadPlyFile(object_path.string().c_str());

    Geometry<PCNVertex>::sptr_type bbox_geo = makeWireframeCubeGeometry();

    Shader::sptr_type unlit_vertex_shader = std::make_shared<Shader>(GL_VERTEX_SHADER, Shader::unlit_vertex_shader_source);
    Shader::sptr_type unlit_fragment_shader = std::make_shared<Shader>(GL_FRAGMENT_SHADER, Shader::unlit_fragment_shader_source);
    Shader::sptr_type lit_vertex_shader = std::make_shared<Shader>(GL_VERTEX_SHADER, Shader::lit_vertex_shader_source);
    Shader::sptr_type lit_fragment_shader = std::make_shared<Shader>(GL_FRAGMENT_SHADER, Shader::lit_fragment_shader_source);
    Program::sptr_type unlit_program = std::make_shared<Program>(unlit_vertex_shader, unlit_fragment_shader);
    Program::sptr_type lit_program = std::make_shared<Program>(lit_vertex_shader, lit_fragment_shader);

    Mesh::sptr_type object = std::make_shared<Mesh>(object_geo, lit_program);
    SCENE.addMesh(object);

    Body object_body;
    object_body.m_vel_mag = 0.5;

    Mesh::sptr_type bbox = std::make_shared<Mesh>(bbox_geo, unlit_program);
    bbox->setTransform(glm::scale(bbox->getTransform(), glm::vec3(10.0f, 10.0f, 10.0f)));
    SCENE.addMesh(bbox);

    Camera &camera = SCENE.getCamera();
    camera.setFocusPoint(glm::vec3(0.0, 0.0, 0.0));
    camera.setPosition(glm::vec3(0.0, 0.0, 30.0));

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, pixel_width, pixel_height);

    glfwSetKeyCallback(window, keypress);
    glfwSetCursorPosCallback(window, mouse_move);
    glfwSetMouseButtonCallback(window, mouse_click);
    glfwSetScrollCallback(window, mouse_scroll);

    auto ptime = steady_clock::now();

    while (!glfwWindowShouldClose(window)) {
        auto time = steady_clock::now();
        duration<float> ticks = time - ptime;
        ptime = time;

        object_body.update(ticks.count());
        object->setTransform(object_body.baseModelView(glm::mat4x4(1)));

        // render.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        SCENE.render();
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

void initglad() {
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
    SCENE.getCamera().zoom(yoffset);
}

void mouse_move(GLFWwindow *wnd, double xpos, double ypos) {
    if (ROTATING) {
        double theta = -1 * 2 * M_PI * ((xpos - MOUSE_X) / SCENE.getViewportWidth());
        double phi = -1 * M_PI * ((ypos - MOUSE_Y) / SCENE.getViewportHeight());

        SCENE.getCamera().rotate(theta, phi);
        MOUSE_X = xpos;
        MOUSE_Y = ypos;
    }
}
