// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "graphplay.h"
#include "Driver.h"

#include <random>

#include <glm/mat4x4.hpp>

namespace graphplay {
    const fzx::Body::sptr_type ORIGIN = std::make_shared<fzx::Body>();
    std::random_device RANDOM_DEV;
    std::default_random_engine RANDOM_ENG(RANDOM_DEV());

    GPObject::GPObject(gfx::Geometry<gfx::PCNVertex>::sptr_type geo, gfx::Program::sptr_type program)
        : geometry(geo)
    {
        std::uniform_real_distribution<float> random_unit(-10.0f, 10.0f);

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
        body->boundingBox(geo->boundingBox());
        body->addConstraint(fzx::AttachedSpring(0.7f, *ORIGIN));
    }

    void GPObject::update(float alpha) {
        glm::mat4x4 model_xform = body->modelTransformation(alpha, glm::mat4x4(1));
        mesh->modelTransformation(model_xform);
    }

    BoundedGPObject::BoundedGPObject(
        gfx::Geometry<gfx::PCNVertex>::sptr_type geo,
        gfx::Program::sptr_type object_program,
        gfx::Program::sptr_type bbox_program)
        : object(geo, object_program)
    {
        bbox_geometry = gfx::makeBoundingBoxGeometry(geo->boundingBox());
        bbox_mesh = std::make_shared<gfx::Mesh>(bbox_geometry, bbox_program);
    }

    void BoundedGPObject::update(float alpha) {
        glm::mat4x4 model_xform = object.body->modelTransformation(
            alpha, glm::mat4x4(1));
        object.mesh->modelTransformation(model_xform);
        bbox_mesh->modelTransformation(model_xform);
    }
}
