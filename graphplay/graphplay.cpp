// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "graphplay.h"
#include "config.h"

#include <chrono>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <thread>

#include <boost/filesystem.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

#include "Body.h"
#include "Constraint.h"
#include "Geometry.h"
#include "Mesh.h"
#include "PhysicsSystem.h"
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

const duration<float> FRAME_RATE(1.0f / 60.0f);
const float TIME_STEP = FRAME_RATE.count() / 5;

Scene SCENE(1024, 768);
double MOUSE_X = 0.0, MOUSE_Y = 0.0;
bool ROTATING = false;

int main(int argc, char **argv) {
    int pixel_width = SCENE.getViewportWidth(), pixel_height = SCENE.getViewportHeight();
    GLFWwindow *window = nullptr;
    std::random_device random_dev;
    std::default_random_engine random_eng(random_dev());
    std::uniform_real_distribution<float> random_unit(-10.0f, 10.0f);

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

    Program::sptr_type unlit_program = createUnlitProgram();
    Program::sptr_type lit_program = createLitProgram();

    Mesh::sptr_type object = std::make_shared<Mesh>(object_geo, lit_program);
    SCENE.addMesh(object);

    Mesh::sptr_type bbox = std::make_shared<Mesh>(bbox_geo, unlit_program);
    bbox->modelTransformation(glm::scale(bbox->modelTransformation(), glm::vec3(10.0f, 10.0f, 10.0f)));
    SCENE.addMesh(bbox);

    Body::sptr_type origin_body = std::make_shared<Body>();
    Body::sptr_type object_body = std::make_shared<Body>();
    object_body->position({ 10.0, 0.0, 0.0 });
    object_body->addConstraint(AttachedSpring(0.7, *origin_body));
    // object_body->velocity({ 1.5, 0.3, 0.0 });
    // object_body->angularVelocity({ M_PI_2, M_PI_4, 0.0 });
    std::cout << "object = " << *object_body << std::endl;

    PhysicsSystem physics(TIME_STEP);
    physics.addBody(object_body);

    Camera &camera = SCENE.getCamera();
    camera.focusPoint(glm::vec3(0.0, 0.0, 0.0));
    camera.position(glm::vec3(0.0, 0.0, 30.0));

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, pixel_width, pixel_height);

    glfwSetKeyCallback(window, keypress);
    glfwSetCursorPosCallback(window, mouse_move);
    glfwSetMouseButtonCallback(window, mouse_click);
    glfwSetScrollCallback(window, mouse_scroll);

    auto ptime = steady_clock::now();

    while (!glfwWindowShouldClose(window)) {
        auto frame_time = steady_clock::now();
        duration<float> frame_seconds = frame_time - ptime;
        ptime = frame_time;

        // glm::vec3 gust(random_unit(random_eng), random_unit(random_eng), random_unit(random_eng));
        // object_body->addForce(gust);

        // glm::vec3 displacement = object_body->position();
        // glm::vec3 restoring = displacement * 1.5f * -1.0f;
        // object_body->addForce(restoring);

        float alpha = physics.update(frame_seconds.count());
        object->modelTransformation(object_body->modelTransformation(alpha, glm::mat4x4(1)));

        // render.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        SCENE.render();
        glfwSwapBuffers(window);
        glfwPollEvents();

        auto update_time = steady_clock::now();
        duration<float> update_seconds = update_time - frame_time;
        duration<float> sleep_seconds = FRAME_RATE - update_seconds;
        std::this_thread::sleep_for(sleep_seconds);
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
