// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <glm/glm.hpp>

namespace graphplay {
    class Camera {
    public:
        Camera();
        Camera(const glm::vec3 &location, const glm::vec3 &direction, const glm::vec3 &up);

        glm::mat4x4 getViewTransform() const;

        inline void setLocation(const glm::vec3 &new_location) { m_location = new_location; }
        inline void setDirection(const glm::vec3 &new_direction) { m_direction = new_direction; }
        inline void setUpDirection(const glm::vec3 &new_up) { m_up = new_up; }

    private:
        glm::vec3 m_location, m_direction, m_up;
    };
};
