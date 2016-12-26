// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_DRIVER_H_
#define _GRAPHPLAY_GRAPHPLAY_DRIVER_H_

#include "graphplay.h"

#include "fzx/Body.h"
#include "gfx/Geometry.h"
#include "gfx/Shader.h"
#include "gfx/Mesh.h"

namespace graphplay {
    extern const fzx::Body::sptr_type ORIGIN;

    class GPObject {
    public:
        GPObject(gfx::Geometry<gfx::PCNVertex>::sptr_type geo, gfx::Program::sptr_type program);
        void update(float alpha);

        gfx::Geometry<gfx::PCNVertex>::sptr_type geometry;
        gfx::Mesh::sptr_type mesh;
        fzx::Body::sptr_type body;
    };

    class BoundedGPObject {
    public:
        BoundedGPObject(
            gfx::Geometry<gfx::PCNVertex>::sptr_type geo,
            gfx::Program::sptr_type object_program,
            gfx::Program::sptr_type bbox_program);
        void update(float alpha);

        GPObject object;
        gfx::MutableGeometry<gfx::PCNVertex>::sptr_type bbox_geometry;
        gfx::Mesh::sptr_type bbox_mesh;
    };

    // class Driver {
    // public:
    // private:
    // };
}

#endif
