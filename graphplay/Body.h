#ifndef _BODY_H_
#define _BODY_H_

#include <glm/glm.hpp>

class Body
{
public:
    Body();
    ~Body();

    // The position of the object in "world" coordinates.
    glm::vec3 mw_pos;

    // The velocity of the object in "world" coordinates, as a unit vector, 
    // and a float for its magnitude.
    glm::vec3 mw_vel_dir;
    float m_vel_mag;

    // Angular quantities: The angular position, angular velocity, and the
    // axis around which it's rotating.
    float m_ang_pos, m_ang_vel;
    glm::vec3 mw_ang_vel_dir;

    virtual void render(const glm::mat4x4 &projection, const glm::mat4x4 &model_view, int flags) = 0;
};

#endif
