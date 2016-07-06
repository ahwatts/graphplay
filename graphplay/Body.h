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
    class Phase {
    public:
        Phase();
        Phase(const glm::vec3 &pos, const glm::vec3 &momentum);
        glm::vec3 position, momentum;
    };

    Phase operator+(const Phase &p1, const Phase &p2);
    Phase operator*(const Phase &p, float t);
    Phase& operator+=(Phase &p, const Phase &dp);
    Phase& operator*=(Phase &p, float dt);
    std::ostream& operator<<(std::ostream &stream, const Phase &body);

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

        // jerk, i.e, the derivative of acceleration.
        glm::vec3 jerk() const;

        void update(float dt);

        glm::mat4x4 modelTransformation(const glm::mat4x4 &base_transform) const;

        friend std::ostream& operator<<(std::ostream &stream, const Body &body);

    protected:
        float m_mass;
        glm::vec3 m_position, m_velocity;
        glm::vec3 m_force, m_prev_force, m_jerk;
        typename FirstOrderODE<Phase, float>::sptr_type m_equation;
        typename Integrator<Phase, float>::uptr_type m_integrator;
    };

    std::ostream& operator<<(std::ostream &stream, const Body &body);

    class BodyStateEquation : public FirstOrderODE<Phase, float> {
    public:
        BodyStateEquation(const Body &body);
        virtual Phase operator()(Phase pos, float base_time, float step_time) const;

    protected:
        const Body &m_body;
    };
};

#endif
