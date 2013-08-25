// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <glm/glm.hpp>

namespace graphplay {
    class Camera {
    public:
        Camera();
        Camera(const glm::vec3 &location, const glm::vec3 &direction, const glm::vec3 &up);

        glm::mat4x4 getViewTransform() const;

        void setLocation(const glm::vec3 &new_location);
        void setDirection(const glm::vec3 &new_direction);
        void setUpDirection(const glm::vec3 &new_up);

    private:
        glm::vec3 m_location, m_direction, m_up;
    };
};
