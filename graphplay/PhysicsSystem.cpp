// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*

#include "PhysicsSystem.h"

namespace graphplay {
    PhysicsSystem::PhysicsSystem(float time_step)
        : m_time_step(time_step),
          m_bodies()
    {}

    PhysicsSystem::~PhysicsSystem() {}

    void PhysicsSystem::addBody(Body::sptr_type new_body) {
        for (auto&& weak_body : m_bodies) {
            if (weak_body.lock() == new_body) {
                return;
            }
        }

        m_bodies.emplace_back(new_body);
    }

    void PhysicsSystem::update(float total_step) {
        float remaining = total_step;
        while (remaining > 0) {
            stepTime(nullptr);
            remaining -= m_time_step;
        }
    }

    void PhysicsSystem::stepTime(float *step_size) {
        float local_step_size = m_time_step;

        if (step_size != nullptr) {
            local_step_size = *step_size;
        }

        for (auto&& weak_body : m_bodies) {
            Body::sptr_type body = weak_body.lock();
            if (body) {
                body->update(local_step_size);
            }
        }
    }
}
