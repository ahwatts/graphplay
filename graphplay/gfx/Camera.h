// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_GFX_CAMERA_H_
#define _GRAPHPLAY_GRAPHPLAY_GFX_CAMERA_H_

#include "../graphplay.h"

#include <memory>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace graphplay {
    namespace gfx {
        class Camera {
        public:
            Camera();
            Camera(const glm::vec3 &position, const glm::vec3 &focus);

            glm::vec3 focusPoint() const;
            void focusPoint(const glm::vec3 &new_focus);

            glm::vec3 position() const;
            void position(const glm::vec3 &new_position);

            void reset();
            void rotate(double dtheta, double dphi);
            void zoom(double dr);

            glm::mat4x4 viewTransformation() const;

            static const glm::vec3 DEFAULT_POSITION, DEFAULT_FOCUS_POINT;

        private:
            glm::vec3 m_focus_point, m_position;
            // double m_radius, m_theta, m_phi;
        };

        typedef std::unique_ptr<Camera> up_Camera;
        typedef std::shared_ptr<Camera> sp_Camera;
        typedef std::weak_ptr<Camera> wp_Camera;
    }
}

#endif
