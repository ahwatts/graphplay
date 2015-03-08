// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_CAMERA_H_
#define _GRAPHPLAY_GRAPHPLAY_CAMERA_H_

#include <memory>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace graphplay {
    class Camera {
    public:
        Camera();
        Camera(const glm::vec3 &position, const glm::vec3 &focus);
        Camera(const Camera &other);
        Camera(Camera &&other);

        Camera& operator=(const Camera &other);
        Camera& operator=(Camera &&other);

        inline const glm::vec3& getFocusPoint() const { return m_focus_point; }
        void setFocusPoint(const glm::vec3 &new_focus);

        inline const glm::vec3& getPosition() const { return m_position; }
        void setPosition(const glm::vec3 &new_position);

        void reset();
        void rotate(double dtheta, double dphi);
        void zoom(double dr);

        glm::mat4x4 getViewTransform() const;

        static const glm::vec3 DEFAULT_POSITION, DEFAULT_FOCUS_POINT;

    private:
        glm::vec3 m_focus_point, m_position;
        // double m_radius, m_theta, m_phi;
    };

    typedef std::unique_ptr<Camera> up_Camera;
    typedef std::shared_ptr<Camera> sp_Camera;
    typedef std::weak_ptr<Camera> wp_Camera;
};

#endif
