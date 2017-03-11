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

struct GLFWwindow;

namespace graphplay {
    class Input;

    extern const fzx::Body::sptr_type ORIGIN;
    extern const std::chrono::duration<float> FRAME_RATE;
    extern const float TIME_STEP;

    struct Context {
        Input *input;
    };

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

    void drive(GLFWwindow *window);
}

#endif
