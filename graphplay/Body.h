// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_BODY_H_
#define _GRAPHPLAY_GRAPHPLAY_BODY_H_

#include <iostream>
#include <memory>

#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "Integrator.h"

namespace graphplay {
    class Body
    {
    public:
        typedef std::unique_ptr<Body> uptr_type;
        typedef std::shared_ptr<Body> sptr_type;
        typedef std::weak_ptr<Body> wptr_type;

        Body();
        Body(const glm::vec3 &pos, const glm::vec3 &vel);

        glm::vec3 position() const;
        void position(const glm::vec3& new_pos);

        // glm::quat orientation() const;
        // void orientation(const glm::quat &new_orientation);

        glm::vec3 velocity() const;
        void velocity(const glm::vec3 &new_vel);

        // glm::vec3 angularVelocity() const;
        // void angularVelocity(const glm::vec3 &new_ang_vel);

        void update(float dt);

        glm::mat4x4 modelTransformation(const glm::mat4x4 &base_transform) const;

        friend std::ostream& operator<<(std::ostream &stream, const Body &body);

    protected:
        glm::vec3 m_position, m_velocity;
        // Rk4<glm::vec3, float> m_integrator;

        // glm::quat m_orientation;
        // glm::vec3 m_angular_velocity;
    };

    std::ostream& operator<<(std::ostream &stream, const Body &body);
};

#endif
