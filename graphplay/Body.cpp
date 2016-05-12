// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "graphplay.h"
#include "Body.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

namespace graphplay {
    Body::Body()
        : m_position(),
          m_velocity_dir(1.0, 0.0, 0.0),
          m_velocity_mag(0),
          m_orientation(),
          m_angular_velocity(0)
    {}

    Body::Body(const glm::vec3 &pos, const glm::vec3 &vel) : Body() {
        setPosition(pos);
        setVelocity(vel);
    }

    glm::vec3 Body::position() {
        return m_position;
    }

    void Body::setPosition(const glm::vec3 &new_pos) {
        m_position = new_pos;
    }

    glm::quat Body::orientation() {
        return m_orientation;
    }

    void Body::setOrientation(const glm::quat &new_orientation) {
        m_orientation = glm::normalize(new_orientation);
    }

    glm::vec3 Body::velocity() {
        return m_velocity_dir * m_velocity_mag;
    }

    void Body::setVelocity(const glm::vec3 &new_vel) {
        m_velocity_mag = glm::length(new_vel);
        if (m_velocity_mag != 0.0) {
            m_velocity_dir = new_vel / m_velocity_mag;
        } else {
            m_velocity_dir = { 1.0, 0.0, 0.0 };
        }
    }

    glm::vec3 Body::angularVelocity() {
        return m_angular_velocity;
    }

    void Body::setAngularVelocity(const glm::vec3 &new_ang_vel) {
        m_angular_velocity = new_ang_vel;
    }

    void Body::update(float dt) {
        float ds_mag = dt * m_velocity_mag;
        m_position += m_velocity_dir * ds_mag;

        glm::quat w(0.0, m_angular_velocity);
        glm::quat dq = w * 0.5f * m_orientation;
        m_orientation = glm::normalize(m_orientation + dq);
    }

    glm::mat4x4 Body::modelview(const glm::mat4x4 &base_modelview) {
        glm::mat4x4 transform = base_modelview;
        // transform = glm::translate(transform, m_position);
        transform = glm::mat4_cast(m_orientation) * transform;
        return transform;
    }

    std::ostream& operator<<(std::ostream &stream, const Body &body) {
        return stream << "Body { "
                      << "m_position = "     << body.m_position     << ", "
                      << "m_velocity_dir = " << body.m_velocity_dir << ", "
                      << "m_velocity_mag = " << body.m_velocity_mag << ", "
                      << "m_orientation = "  << body.m_orientation  << " }";
    }
};
