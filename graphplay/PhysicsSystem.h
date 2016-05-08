// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_PHYSICS_SYSTEM_H_
#define _GRAPHPLAY_GRAPHPLAY_PHYSICS_SYSTEM_H_

#include <vector>

#include "Body.h"

namespace graphplay {
    class PhysicsSystem {
    public:
        PhysicsSystem();
        PhysicsSystem(const PhysicsSystem&) = delete;
        PhysicsSystem(PhysicsSystem&&) = delete;
        ~PhysicsSystem();

        PhysicsSystem& operator=(const PhysicsSystem&) = delete;
        PhysicsSystem& operator=(PhysicsSystem&&) = delete;

        std::vector<Body::wptr_type>& bodies() { return m_bodies; }

        void addBody(Body::sptr_type new_body);
        void update(float dt);

    protected:
        std::vector<Body::wptr_type> m_bodies;
    };
}

#endif
