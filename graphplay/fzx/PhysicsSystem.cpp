// -*- mode: c++; indent-tabs-mode: nil -*

#include "../graphplay.h"
#include "PhysicsSystem.h"

#include <iostream>

#include <glm/gtx/io.hpp>

namespace graphplay {
    namespace fzx {
        PhysicsSystem::PhysicsSystem(float time_step)
            : m_time_step(time_step),
              m_hangover_time(0.0),
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

        float PhysicsSystem::update(float total_step) {
            total_step = total_step - m_hangover_time;
            float simulated = 0.0;
            int steps = 0;

            // Simulate up until we go over the total time step.
            while (simulated < total_step) {
                stepTime();
                simulated += m_time_step;
                steps += 1;
            }

            m_hangover_time = simulated - total_step;
            return (m_time_step - m_hangover_time) / m_time_step;
        }

        void PhysicsSystem::stepTime() {
            for (auto&& weak_body : m_bodies) {
                Body::sptr_type body = weak_body.lock();
                if (body) {
                    body->update(m_time_step);
                }
            }

            detectCollisions();
        }

        void PhysicsSystem::detectCollisions() {
            // for (auto&& weak_body1 : m_bodies) {
            //     for (auto&& weak_body2 : m_bodies) {
            //         Body::sptr_type body1 = weak_body1.lock();
            //         Body::sptr_type body2 = weak_body2.lock();

            //         if (body1 && body2 && body1 != body2 &&
            //             body1->boundingBox().collides(body2->boundingBox())) {
            //             std::cout << "Collision detected! body1 = " << body1 << " body2 = " << body2
            //                       << " body1 bbox min = " << body1->boundingBox().min
            //                       << " body2 bbox min = " << body2->boundingBox().min
            //                       << std::endl;
            //         }
            //     }
            // }
        }
    }
}
