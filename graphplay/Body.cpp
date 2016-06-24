// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "graphplay.h"
#include "Body.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

namespace graphplay {
    Body::Body()
        : m_mass(1.0),
          m_position(),
          m_velocity(),
          m_force()
    {}

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

    void Body::update(float dt) {
        glm::vec3 q0 = m_position;
        glm::vec3 p0 = m_velocity * m_mass;
        // std::cout << "q0   = " << q0   << " p0   = " << p0   << std::endl;

        glm::vec3 qdot = p0 / m_mass;
        glm::vec3 pdot = m_force;
        // std::cout << "qdot = " << qdot << " pdot = " << pdot << std::endl;

        glm::vec3 dq = qdot * dt;
        glm::vec3 dp = pdot * dt;
        // std::cout << "dq   = " << dq   << " dp   = " << dp   << std::endl;

        glm::vec3 q1 = q0 + dq;
        glm::vec3 p1 = p0 + dp;
        // std::cout << "q1   = " << q1   << " p1   = " << p1   << std::endl;
        // std::cout << std::endl;

        m_velocity = p1 / m_mass;
        m_position = q1;

        // Set the force to zero fo the next iteration.
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
