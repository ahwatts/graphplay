#ifndef _WORLD_H_
#define _WORLD_H_

#include <glm/glm.hpp>

class World
{
public:
    World(float vp_width, float vp_height);
    ~World();

    void update(float dt);
    void render();

private:
    glm::mat4x4 perspective;
    glm::mat4x4 model_view;
    float vp_width, vp_height;
};

#endif
