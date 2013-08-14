// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_WORLD_H_
#define _GRAPHPLAY_GRAPHPLAY_WORLD_H_

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "Mesh.h"

namespace graphplay {
    class Scene
    {
    public:
        Scene(unsigned int vp_width, unsigned int vp_height);
        ~Scene();

        void setViewport(unsigned int new_width, unsigned int new_height);

        void addMesh(wp_Mesh mesh);
        wp_Mesh removeMesh(wp_Mesh &mesh);

        void render();

    private:
        glm::mat4x4 m_perspective;
        glm::mat4x4 m_model_view;
        unsigned int m_vp_width, m_vp_height;

        std::vector<wp_Mesh> m_meshes;
    };

    typedef std::unique_ptr<Scene> up_Scene;
    typedef std::shared_ptr<Scene> sp_Scene;
    typedef std::weak_ptr<Scene> wp_Scene;
};

#endif
