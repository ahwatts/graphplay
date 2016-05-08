// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*

#include "PhysicsSystem.h"

namespace graphplay {
    PhysicsSystem::PhysicsSystem() : m_bodies() {}
    PhysicsSystem::~PhysicsSystem() {}

    void PhysicsSystem::addBody(Body::sptr_type new_body) {
        for (auto&& weak_body : m_bodies) {
            if (weak_body.lock() == new_body) {
                return;
            }
        }

        m_bodies.emplace_back(new_body);
    }

    void PhysicsSystem::update(float dt) {
        for (auto&& weak_body : m_bodies) {
            Body::sptr_type body = weak_body.lock();
            if (body) {
                body->update(dt);
            }
        }
    }
}
