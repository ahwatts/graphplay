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
Body::sptr_type ORIGIN = std::make_shared<Body>();
double MOUSE_X = 0.0, MOUSE_Y = 0.0;
bool ROTATING = false;
std::random_device RANDOM_DEV;
std::default_random_engine RANDOM_ENG(RANDOM_DEV());

class GPObject {
public:
    GPObject(Geometry<PCNVertex>::sptr_type geo, Program::sptr_type program)
        : geometry(geo)
    {
        std::uniform_real_distribution<float> random_unit(-10.0f, 10.0f);

        mesh = std::make_shared<Mesh>(geo, program);
        body = std::make_shared<Body>();
        body->position({
            random_unit(RANDOM_ENG),
            random_unit(RANDOM_ENG),
            random_unit(RANDOM_ENG)
        });
        body->velocity({
            random_unit(RANDOM_ENG) / 2,
            random_unit(RANDOM_ENG) / 2,
            random_unit(RANDOM_ENG) / 2
        });
        body->addConstraint(AttachedSpring(0.7f, *ORIGIN));
    }

    void update(float alpha) {
        glm::mat4x4 model_xform = body->modelTransformation(alpha, glm::mat4x4(1));
        mesh->modelTransformation(model_xform);
    }

    Geometry<PCNVertex>::sptr_type geometry;
    Mesh::sptr_type mesh;
    Body::sptr_type body;
};

class BoundedGPObject {
public:
    BoundedGPObject(Geometry<PCNVertex>::sptr_type geo, Program::sptr_type object_program, Program::sptr_type bbox_program)
        : object(geo, object_program)
    {
        bbox_geometry = makeBoundingBoxGeometry(geo->boundingBox());
        bbox_mesh = std::make_shared<Mesh>(bbox_geometry, bbox_program);
    }

    void update(float alpha) {
        glm::mat4x4 model_xform = object.body->modelTransformation(alpha, glm::mat4x4(1));
        object.mesh->modelTransformation(model_xform);
        bbox_mesh->modelTransformation(model_xform);
    }

    GPObject object;
    MutableGeometry<PCNVertex>::sptr_type bbox_geometry;
    Mesh::sptr_type bbox_mesh;
};

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

    path assets_path("assets");
    path bunny_path(assets_path);
    bunny_path /= "stanford_bunny.ply";
    path armadillo_path(assets_path);
    armadillo_path /= "stanford_armadillo.ply";

    Program::sptr_type unlit_program = createUnlitProgram();
    Program::sptr_type lit_program = createLitProgram();

    BoundedGPObject octohedron(makeOctohedronGeometry(), unlit_program, unlit_program);
    BoundedGPObject sphere(makeSphereGeometry(), lit_program, unlit_program);
    BoundedGPObject bunny(loadPlyFile(bunny_path.string().c_str()), lit_program, unlit_program);
    BoundedGPObject armadillo(loadPlyFile(armadillo_path.string().c_str()), lit_program, unlit_program);
    GPObject bbox(makeWireframeCubeGeometry(), unlit_program);

    SCENE.addMesh(octohedron.object.mesh);
    SCENE.addMesh(octohedron.bbox_mesh);
    SCENE.addMesh(sphere.object.mesh);
    SCENE.addMesh(sphere.bbox_mesh);
    SCENE.addMesh(bunny.object.mesh);
    SCENE.addMesh(bunny.bbox_mesh);
    SCENE.addMesh(armadillo.object.mesh);
    SCENE.addMesh(armadillo.bbox_mesh);
    SCENE.addMesh(bbox.mesh);

    bbox.mesh->modelTransformation(glm::scale(bbox.mesh->modelTransformation(), glm::vec3(10.0f, 10.0f, 10.0f)));

    PhysicsSystem physics(TIME_STEP);
    physics.addBody(octohedron.object.body);
    physics.addBody(sphere.object.body);
    physics.addBody(bunny.object.body);
    physics.addBody(armadillo.object.body);

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

        // Update physics; set model transformations.
        float alpha = physics.update(frame_seconds.count());
        octohedron.update(alpha);
        sphere.update(alpha);
        bunny.update(alpha);
        armadillo.update(alpha);

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
