// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "graphplay.h"
#include "Body.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

namespace graphplay {
    Body::Body()
        : m_position(),
          m_velocity_dir(1.0, 0.0, 0.0),
          m_velocity_mag(0)
    {}

    Body::Body(const glm::vec3 &pos, const glm::vec3 &vel) : Body() {
        setPosition(pos);
        setVelocity(vel);
    }

    glm::vec3 Body::position() {
        return m_position;
    }

    void Body::setPosition(const glm::vec3& new_pos) {
        m_position = new_pos;
    }

    glm::vec3 Body::velocity() {
        return m_velocity_dir * m_velocity_mag;
    }

    void Body::setVelocity(const glm::vec3& new_vel) {
        m_velocity_mag = glm::length(new_vel);
        if (m_velocity_mag != 0.0) {
            m_velocity_dir = new_vel / m_velocity_mag;
        } else {
            m_velocity_dir = { 1.0, 0.0, 0.0 };
        }
    }

    void Body::update(float dt) {
        float ds_mag = dt * m_velocity_mag;
        m_position += m_velocity_dir * ds_mag;
    }

    glm::mat4 Body::modelview(const glm::mat4 &base_modelview) {
        return glm::translate(base_modelview, m_position);
    }

    std::ostream& operator<<(std::ostream &stream, const Body &body) {
        return stream << "Body { "
                      << "m_position = " << body.m_position << ", "
                      << "m_velocity_dir = " << body.m_velocity_dir << ", "
                      << "m_velocity_mag = " << body.m_velocity_mag << " }";
    }
};
