// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"

namespace graphplay {
    Camera::Camera() : m_location(), m_direction(), m_up() { }

    glm::mat4x4 Camera::getViewTransform() const {
        return glm::lookAt(m_location, m_direction, m_up);
    }
};
