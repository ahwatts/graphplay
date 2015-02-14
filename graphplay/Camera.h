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
        Camera(const glm::vec3 &location, const glm::vec3 &direction, const glm::vec3 &up);
        Camera(const Camera &other);
        Camera(Camera &&other);

        Camera& operator=(const Camera &other);
        Camera& operator=(Camera &&other);

        glm::mat4x4 getViewTransform() const;

        inline void setLocation(const glm::vec3 &new_location) { m_location = new_location; }
        inline void setDirection(const glm::vec3 &new_direction) { m_direction = new_direction; }
        inline void setUpDirection(const glm::vec3 &new_up) { m_up = new_up; }

    private:
        glm::vec3 m_location, m_direction, m_up;
    };

    typedef std::unique_ptr<Camera> up_Camera;
    typedef std::shared_ptr<Camera> sp_Camera;
    typedef std::weak_ptr<Camera> wp_Camera;
};

#endif
