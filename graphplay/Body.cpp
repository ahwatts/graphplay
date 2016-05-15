// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "graphplay.h"
#include "Body.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

namespace graphplay {
    Body::Body()
        : m_position(),
          m_velocity()
          // m_orientation(),
          // m_angular_velocity(0)
    {}

    Body::Body(const glm::vec3 &pos, const glm::vec3 &vel) : Body() {
        position(pos);
        velocity(vel);
    }

    glm::vec3 Body::position() const {
        return m_position;
    }

    void Body::position(const glm::vec3 &new_pos) {
        m_position = new_pos;
    }

    // glm::quat Body::orientation() const {
    //     return m_orientation;
    // }

    // void Body::orientation(const glm::quat &new_orientation) {
    //     m_orientation = glm::normalize(new_orientation);
    // }

    glm::vec3 Body::velocity() const {
        return m_velocity;
    }

    void Body::velocity(const glm::vec3 &new_vel) {
        m_velocity = new_vel;
        // m_velocity_mag = glm::length(new_vel);
        // if (m_velocity_mag != 0.0) {
        //     m_velocity_dir = new_vel / m_velocity_mag;
        // } else {
        //     m_velocity_dir = { 1.0, 0.0, 0.0 };
        // }
    }

    // glm::vec3 Body::angularVelocity() const {
    //     return m_angular_velocity;
    // }

    // void Body::angularVelocity(const glm::vec3 &new_ang_vel) {
    //     m_angular_velocity = new_ang_vel;
    // }

    void Body::update(float dt) {
        glm::vec3 ds = m_velocity * dt;
        m_position += ds;

        // glm::quat w(0.0, m_angular_velocity);
        // glm::quat dq = w * m_orientation * 0.5f * dt;
        // m_orientation = glm::normalize(m_orientation + dq);
    }

    glm::mat4x4 Body::modelTransformation(const glm::mat4x4 &base_modelview) const {
        glm::mat4x4 transform = glm::translate(base_modelview, m_position);
        // transform = transform * glm::mat4_cast(m_orientation);
        return transform;
    }

    std::ostream& operator<<(std::ostream &stream, const Body &body) {
        return stream << "Body:" << std::endl
            << "  m_position = " << body.m_position << std::endl
            << "  m_velocity = " << body.m_velocity << std::endl;
            // << "  m_orientation = " << body.m_orientation << std::endl
            // << "  m_angular_velocity = " << body.m_angular_velocity << std::endl;
    }
};
