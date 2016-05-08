// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_BODY_H_
#define _GRAPHPLAY_GRAPHPLAY_BODY_H_

#include <memory>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace graphplay {
    class Body
    {
    public:
        typedef std::unique_ptr<Body> uptr_type;
        typedef std::shared_ptr<Body> sptr_type;
        typedef std::weak_ptr<Body> wptr_type;

        Body();
        virtual ~Body();

        // The position of the object in "world" coordinates.
        glm::vec3 mw_pos;

        // The velocity of the object in "world" coordinates, as a unit vector, 
        // and a float for its magnitude.
        glm::vec3 mw_vel_dir;
        float m_vel_mag;

        // Angular quantities: The angular position, angular velocity, and the
        // axis around which it's rotating.
        float m_ang_pos, m_ang_vel;
        glm::vec3 mw_ang_vel_dir;

        void update(float dt);
        glm::mat4 baseModelView(const glm::mat4 &wld_model_view);
    };
};

#endif
