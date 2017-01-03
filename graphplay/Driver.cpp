// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "graphplay.h"
#include "Driver.h"

#include <random>
#include <thread>

#include <boost/filesystem.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

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
        // body->boundingBox(geo->boundingBox());
        body->addConstraint(fzx::AttachedSpring(0.7f, *ORIGIN));
    }

    void GPObject::update(float alpha) {
        glm::mat4x4 model_xform = body->modelTransformation(
            alpha, glm::mat4x4(1));
        mesh->modelTransformation(model_xform);
    }

    // BoundedGPObject::BoundedGPObject()
    //     : object(),
    //       bbox_geometry(),
    //       bbox_mesh()
    // {}

    // BoundedGPObject::BoundedGPObject(GPObject &object, gfx::Program::sptr_type bbox_program)
    //     : object(object),
    //       bbox_geometry(gfx::makeBoundingBoxGeometry(object.geometry->boundingBox())),
    //       bbox_mesh(std::make_shared<gfx::Mesh>(bbox_geometry, bbox_program))
    // {}

    // void BoundedGPObject::init(GPObject &object, gfx::Program::sptr_type bbox_program) {
    //     this->object = object;
    //     bbox_geometry = gfx::makeBoundingBoxGeometry(object.geometry->boundingBox());
    //     bbox_mesh = std::make_shared<gfx::Mesh>(bbox_geometry, bbox_program);
    // }

    // void BoundedGPObject::update(float alpha) {
    //     glm::mat4x4 model_xform = object.body->modelTransformation(
    //         alpha, glm::mat4x4(1));
    //     object.mesh->modelTransformation(model_xform);
    //     bbox_mesh->modelTransformation(model_xform);
    // }

    void drive(GLFWwindow *window) {
        int pixel_width, pixel_height;
        glfwGetFramebufferSize(window, &pixel_width, &pixel_height);

        // Create the shader programs.
        gfx::Program::sptr_type unlit_program = gfx::createUnlitProgram();
        gfx::Program::sptr_type lit_program = gfx::createLitProgram();

        // path assets_path("assets");
        // path bunny_path(assets_path);
        // bunny_path /= "stanford_bunny.ply";
        // path armadillo_path(assets_path);
        // armadillo_path /= "stanford_armadillo.ply";

        // Create the objects in the scene.
        GPObject octohedron(gfx::makeOctohedronGeometry(), unlit_program);
        GPObject sphere(gfx::makeSphereGeometry(), lit_program);
        // bunny.object.init(gfx::loadPlyFile(bunny_path.string().c_str()), lit_program);
        // armadillo.object.init(gfx::loadPlyFile(armadillo_path.string().c_str()), lit_program);

        // Create the scene.
        gfx::Scene scene(pixel_width, pixel_height);
        scene.createBuffers();
        scene.addMesh(octohedron.mesh);
        scene.addMesh(sphere.mesh);

        // Create the physics.
        fzx::PhysicsSystem physics(TIME_STEP);
        physics.addBody(octohedron.body);
        physics.addBody(sphere.body);

        auto ptime = steady_clock::now();

        while (!glfwWindowShouldClose(window)) {
            auto frame_time = steady_clock::now();
            duration<float> frame_seconds = frame_time - ptime;
            ptime = frame_time;

            // Update physics; set model transformations.
            float alpha = physics.update(frame_seconds.count());
            octohedron.update(alpha);
            sphere.update(alpha);
            // bunny.update(alpha);
            // armadillo.update(alpha);

            // render.
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            scene.render();
            glfwSwapBuffers(window);
            glfwPollEvents();

            auto update_time = steady_clock::now();
            duration<float> update_seconds = update_time - frame_time;
            duration<float> sleep_seconds = FRAME_RATE - update_seconds;
            std::this_thread::sleep_for(sleep_seconds);
        }
    }
}
