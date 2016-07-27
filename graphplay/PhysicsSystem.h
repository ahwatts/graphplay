// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_PHYSICS_SYSTEM_H_
#define _GRAPHPLAY_GRAPHPLAY_PHYSICS_SYSTEM_H_

#include <vector>

#include "Body.h"

namespace graphplay {
    class PhysicsSystem {
    public:
        PhysicsSystem(float time_step);
        PhysicsSystem(const PhysicsSystem&) = delete;
        PhysicsSystem(PhysicsSystem&&) = delete;
        ~PhysicsSystem();

        PhysicsSystem& operator=(const PhysicsSystem&) = delete;
        PhysicsSystem& operator=(PhysicsSystem&&) = delete;

        std::vector<Body::wptr_type>& bodies() { return m_bodies; }

        void addBody(Body::sptr_type new_body);
        float update(float total_step);

    protected:
        float m_time_step;
        float m_hangover_time;
        std::vector<Body::wptr_type> m_bodies;

        void stepTime();
    };
}

#endif
