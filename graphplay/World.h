#ifndef _WORLD_H_
#define _WORLD_H_

#include <glm/glm.hpp>

class World
{
public:
    World(unsigned int vp_width, unsigned int vp_height);
    ~World();

    void update(float dt);
    void render();

private:
    glm::mat4x4 m_perspective;
    glm::mat4x4 m_model_view;
    unsigned int m_vp_width, m_vp_height;
};

#endif
