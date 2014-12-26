// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace graphplay {
    Camera::Camera()
        : m_location(0.0f, 0.0f, 0.0f),
          m_direction(0.0f, 0.0f, -1.0f),
          m_up(0.0f, 1.0f, 0.0f) { }

    Camera::Camera(const glm::vec3 &location, const glm::vec3 &direction, const glm::vec3 &up)
        : m_location(location),
          m_direction(direction),
          m_up(up) { }

    glm::mat4x4 Camera::getViewTransform() const {
        return glm::lookAt(m_location, m_direction, m_up);
    }
};
