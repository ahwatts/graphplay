// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "graphplay.h"
#include "Body.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

namespace graphplay {
    // Class Phase.
    Phase::Phase()
        : position(),
          momentum()
    {}

    Phase::Phase(const glm::vec3 &pos, const glm::vec3 &mom)
        : position(pos),
          momentum(mom)
    {}

    Phase operator+(const Phase &p1, const Phase &p2) {
        Phase rv(p1);
        return rv += p2;
    }

    Phase operator*(const Phase &p, float t) {
        Phase rv(p);
        return rv *= t;
    }

    Phase& operator+=(Phase &p, const Phase &dp) {
        p.position += dp.position;
        p.momentum += dp.momentum;
        return p;
    }

    Phase& operator*=(Phase &p, float dt) {
        p.position *= dt;
        p.momentum *= dt;
        return p;
    }

    // Class BodyStateEquation.
    BodyStateEquation::BodyStateEquation(const Body &body)
        : m_body(body)
    {}

    Phase BodyStateEquation::operator()(Phase y, float base, float step) const {
        glm::vec3 force = m_body.netForce() + (m_body.jerk() * m_body.mass() * step);
        return Phase(y.momentum / m_body.mass(), force);
    }

    // Class Body.
    Body::Body()
        : m_mass(1.0),
          m_position(),
          m_velocity(),
          m_force(),
          m_prev_force(),
          m_jerk(),
          m_equation(std::make_shared<BodyStateEquation>(*this)),
          m_integrator()
    {
        // m_integrator.reset(new Euler<Phase, float>(m_equation));
         m_integrator.reset(new Rk4<Phase, float>(m_equation));
    }

    Body::Body(float _mass, const glm::vec3 &pos, const glm::vec3 &vel)
        : Body()
    {
        mass(_mass);
        position(pos);
        velocity(vel);
    }

    float Body::mass() const {
        return m_mass;
    }

    void Body::mass(float new_mass) {
        if (new_mass > 0) {
            m_mass = new_mass;
        }
    }

    glm::vec3 Body::position() const {
        return m_position;
    }

    void Body::position(const glm::vec3 &new_pos) {
        m_position = new_pos;
        Phase dependent = m_integrator->dependent();
        dependent.position = new_pos;
        m_integrator->dependent(dependent);
    }

    glm::vec3 Body::velocity() const {
        return m_velocity;
    }

    void Body::velocity(const glm::vec3 &new_vel) {
        m_velocity = new_vel;
    }

    glm::vec3 Body::netForce() const {
        return m_force;
    }

    void Body::addForce(const glm::vec3 &force) {
        m_force += force;
    }

    glm::vec3 Body::jerk() const {
        return m_jerk;
    }

    void Body::update(float dt) {
        // Step the integrator.
        Phase next = m_integrator->step(dt);

        // Update self.
        m_position = next.position;
        m_velocity = next.momentum / m_mass;

        // Update the force bits.
        m_jerk = (m_force - m_prev_force) / dt / m_mass;
        m_prev_force = m_force;
        m_force = { 0.0, 0.0, 0.0 };
    }

    glm::mat4x4 Body::modelTransformation(const glm::mat4x4 &base_modelview) const {
        return glm::translate(base_modelview, m_position);
    }

    std::ostream& operator<<(std::ostream &stream, const Body &body) {
        return stream << "Body:" << std::endl
                      << "  m_position = " << body.m_position << std::endl
                      << "  m_velocity = " << body.m_velocity << std::endl;
    }
};
