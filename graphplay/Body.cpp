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
          m_force(),
          m_prev_force()
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

    void Body::update(float time_step) {
        glm::vec3 impulse = (m_prev_force + m_force) * time_step / 2.0f;
        glm::vec3 dv = impulse / m_mass;

        std::cout << "impulse = " << impulse << std::endl
                  << "delta v = " << dv << std::endl;

        m_velocity += dv;
        m_position += m_velocity * time_step;

        m_prev_force = m_force;
        m_force = glm::vec3();
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
