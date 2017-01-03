// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_DRIVER_H_
#define _GRAPHPLAY_GRAPHPLAY_DRIVER_H_

#include "graphplay.h"

#include <chrono>

#include "fzx/Body.h"
#include "fzx/PhysicsSystem.h"
#include "gfx/Geometry.h"
#include "gfx/Mesh.h"
#include "gfx/Scene.h"
#include "gfx/Shader.h"

namespace graphplay {
    extern const fzx::Body::sptr_type ORIGIN;
    extern const std::chrono::duration<float> FRAME_RATE;
    extern const float TIME_STEP;

    class GPObject {
    public:
        GPObject();
        GPObject(gfx::PCNGeometry::sptr_type geo, gfx::Program::sptr_type program);
        void init(gfx::PCNGeometry::sptr_type geo, gfx::Program::sptr_type program);
        void update(float alpha);

        gfx::PCNGeometry::sptr_type geometry;
        gfx::Mesh::sptr_type mesh;
        fzx::Body::sptr_type body;
    };

    // class BoundedGPObject {
    // public:
    //     BoundedGPObject();
    //     BoundedGPObject(GPObject &gp_object, gfx::Program::sptr_type bbox_program);
    //     void init(GPObject &gp_object, gfx::Program::sptr_type bbox_program);
    //     void update(float alpha);

    //     GPObject object;
    //     gfx::PCNGeometry::sptr_type bbox_geometry;
    //     gfx::Mesh::sptr_type bbox_mesh;
    // };

    void drive(GLFWwindow *window);

    class Driver {
    public:
        
        // Driver(int width, int height, float time_step);
        // void mainLoop(GLFWwindow *window);

        // gfx::Scene scene;
        // fzx::PhysicsSystem physics;
        // gfx::Program::sptr_type unlit_program, lit_program;
        // BoundedGPObject octohedron, sphere, bunny, armadillo;
    };
}

#endif
