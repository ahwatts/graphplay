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
        Body(float mass, const glm::vec3 &pos, const glm::vec3 &vel);

        float mass() const;
        void mass(float new_mass);

        glm::vec3 position() const;
        void position(const glm::vec3& new_pos);

        glm::vec3 velocity() const;
        void velocity(const glm::vec3 &new_vel);

        glm::vec3 netForce() const;
        void addForce(const glm::vec3 &force);

        void update(float dt);

        glm::mat4x4 modelTransformation(const glm::mat4x4 &base_transform) const;

        friend std::ostream& operator<<(std::ostream &stream, const Body &body);

    protected:
        float m_mass;
        glm::vec3 m_position, m_velocity, m_force;
    };

    std::ostream& operator<<(std::ostream &stream, const Body &body);
};

#endif
