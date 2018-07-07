// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "graphplay.h"
#include "Driver.h"

#include <random>
#include <thread>

#include <boost/filesystem.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "Input.h"
#include "gfx/Camera.h"

using namespace boost::filesystem;
using namespace std::chrono;

namespace graphplay {
    const fzx::Body::sptr_type ORIGIN = std::make_shared<fzx::Body>();
    const duration<float> FRAME_RATE(1.0f / 60.0f);
    const float TIME_STEP = FRAME_RATE.count() / 5;

    std::random_device RANDOM_DEV;
    std::default_random_engine RANDOM_ENG(RANDOM_DEV());

    GPObject::GPObject()
        : geometry(),
          mesh(),
          body()
    {}

    GPObject::GPObject(
        gfx::PCNGeometry::sptr_type geo,
        gfx::Program::sptr_type program)
        : GPObject()
    {
        init(geo, program);
    }

    void GPObject::init(
        gfx::PCNGeometry::sptr_type geo,
        gfx::Program::sptr_type program)
    {
        std::uniform_real_distribution<float> random_unit(-10.0f, 10.0f);

        geometry = geo;
        mesh = std::make_shared<gfx::Mesh>(geo, program);
        body = std::make_shared<fzx::Body>();
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
        body->addConstraint(fzx::AttachedSpring(0.7f, *ORIGIN));
    }

    void GPObject::update(float alpha) {
        glm::mat4x4 model_xform = body->modelTransformation(
            alpha, glm::mat4x4(1));
        mesh->modelTransformation(model_xform);
    }

    void drive(GLFWwindow *window) {
        int pixel_width, pixel_height;
        glfwGetFramebufferSize(window, &pixel_width, &pixel_height);

        // Create the shader programs.
        gfx::Program::sptr_type unlit_program = gfx::createUnlitProgram();
        gfx::Program::sptr_type lit_program = gfx::createLitProgram();

        // Find the PLY files.
        path assets_path("assets");
        path bunny_path(assets_path);
        bunny_path /= "stanford_bunny.ply";
        path armadillo_path(assets_path);
        armadillo_path /= "stanford_armadillo.ply";

        // Create the objects in the scene.
        GPObject octohedron(gfx::makeOctohedronGeometry(), unlit_program);
        GPObject icosahedron(gfx::makeIcosahedronGeometry(), unlit_program);
        GPObject sphere(gfx::makeSphereGeometry(), lit_program);
        GPObject bunny(gfx::loadPlyFile(bunny_path.string().c_str()), lit_program);
        GPObject armadillo(gfx::loadPlyFile(armadillo_path.string().c_str()), lit_program);

        // Create the "bounding box" geoemtry.
        GPObject bbox(gfx::makeWireframeCubeGeometry(), unlit_program);
        bbox.mesh->modelTransformation(glm::scale(glm::vec3(10.0, 10.0, 10.0)));

        // Create the scene.
        gfx::Scene scene(pixel_width, pixel_height);
        scene.createBuffers();

        gfx::Camera &camera = scene.getCamera();
        camera.focusPoint(glm::vec3(0.0, 0.0, 0.0));
        camera.position(glm::vec3(0.0, 0.0, 30.0));

        scene.addMesh(octohedron.mesh);
        scene.addMesh(icosahedron.mesh);
        scene.addMesh(sphere.mesh);
        scene.addMesh(bunny.mesh);
        scene.addMesh(armadillo.mesh);
        scene.addMesh(bbox.mesh);

        // Create the physics.
        fzx::PhysicsSystem physics(TIME_STEP);
        physics.addBody(octohedron.body);
        physics.addBody(icosahedron.body);
        physics.addBody(sphere.body);
        physics.addBody(bunny.body);
        physics.addBody(armadillo.body);

        // Set up the input handler.
        Input input{3};
        input.initCallbacks(window);

        // Set the window user pointer.
        Context ctx;
        ctx.input = &input;
        glfwSetWindowUserPointer(window, &ctx);

        auto ptime = steady_clock::now();

        while (!glfwWindowShouldClose(window)) {
            auto frame_time = steady_clock::now();
            duration<float> frame_seconds = frame_time - ptime;
            ptime = frame_time;

            // Update physics; set model transformations.
            float alpha = physics.update(frame_seconds.count());
            octohedron.update(alpha);
            icosahedron.update(alpha);
            sphere.update(alpha);
            bunny.update(alpha);
            armadillo.update(alpha);

            // render.
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            scene.render();
            glfwSwapBuffers(window);

            // handle input.
            input.cloneCurrentState();
            glfwPollEvents();
            if (input.currentState().escape_key) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }

            if (input.currentState().rotating) {
                double dx = input.currentState().mouse_x - input.prevState().mouse_x;
                double dy = input.currentState().mouse_y - input.prevState().mouse_y;
                double theta = -1 * 2 * M_PI * (dx / scene.getViewportWidth());
                double phi = -1 * M_PI * (dy / scene.getViewportHeight());
                scene.getCamera().rotate(theta, phi);
            }

            auto update_time = steady_clock::now();
            duration<float> update_seconds = update_time - frame_time;
            duration<float> sleep_seconds = FRAME_RATE - update_seconds;
            std::this_thread::sleep_for(sleep_seconds);
        }
    }
}
