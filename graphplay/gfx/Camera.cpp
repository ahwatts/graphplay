// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "../graphplay.h"
#include "Camera.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace graphplay {
    namespace gfx {
        const glm::vec3 Camera::DEFAULT_FOCUS_POINT(0.0, 0.0, 0.0);
        const glm::vec3 Camera::DEFAULT_POSITION(1.0, 0.0, 0.0);

        Camera::Camera()
            : m_focus_point(DEFAULT_FOCUS_POINT),
              m_position(DEFAULT_POSITION)
        {}

        Camera::Camera(const glm::vec3 &position, const glm::vec3 &focus_point)
            : m_focus_point(focus_point),
              m_position(position)
        {}

        void Camera::reset() {
            m_focus_point = DEFAULT_FOCUS_POINT;
            m_position = DEFAULT_POSITION;
        }

        void Camera::rotate(double dtheta, double dphi) {
            glm::vec3 y_axis(0.0, 1.0, 0.0);
            glm::vec3 cam_dir = m_position - m_focus_point;
            float radius = glm::length(cam_dir);
            cam_dir = glm::normalize(cam_dir);
        
            cam_dir = glm::rotate(cam_dir, (float)dtheta, y_axis);

            glm::vec3 horizontal = glm::normalize(glm::cross(y_axis, cam_dir));
            cam_dir = glm::rotate(cam_dir, (float)dphi, horizontal);

            float angle = glm::dot(cam_dir, y_axis);
            if (angle > 0.99863 || angle < -0.99863) {
                cam_dir = glm::rotate(cam_dir, (float)(-1 * dphi), horizontal);
            }

            m_position = m_focus_point + cam_dir*radius;
        }

        void Camera::zoom(double dr) {
            glm::vec3 cam_dir = m_position - m_focus_point;
            float radius = glm::length(cam_dir);
            cam_dir = glm::normalize(cam_dir);

            radius += (float)dr;
            if (radius < 1) {
                radius = 1;
            } else if (radius > 100) {
                radius = 100;
            }

            m_position = m_focus_point + cam_dir*radius;
        }

        glm::mat4x4 Camera::viewTransformation() const {
            return glm::lookAt(m_position, m_focus_point, glm::vec3(0.0, 1.0, 0.0));
        }
    }
}
