// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _WORLD_H_
#define _WORLD_H_

#include <vector>
#include <glm/glm.hpp>
#include "Body.h"

class World
{
public:
    World(unsigned int vp_width, unsigned int vp_height);
    ~World();

    void setViewport(unsigned int new_width, unsigned int new_height);
    void update(float dt);
    void render();

private:
    glm::mat4x4 m_perspective;
    glm::mat4x4 m_model_view;
    unsigned int m_vp_width, m_vp_height;
    std::vector<Body*> m_bodies;
};

#endif
