// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_WORLD_H_
#define _GRAPHPLAY_GRAPHPLAY_WORLD_H_

#include <memory>
#include <vector>

#include "Camera.h"
#include "Mesh.h"

namespace graphplay {
    class Scene
    {
    public:
        Scene(unsigned int vp_width, unsigned int vp_height);
        ~Scene();

        void setViewport(unsigned int new_width, unsigned int new_height);

        void addMesh(wp_Mesh mesh);
        wp_Mesh removeMesh(wp_Mesh mesh);
        inline unsigned int getNumMeshes() const { return m_meshes.size(); }

        Camera &getCamera() { return m_camera; }

        void render();

    private:
        glm::mat4x4 m_view;
        glm::mat4x4 m_view_inv;
        glm::mat4x4 m_projection;
        unsigned int m_vp_width, m_vp_height;

        Camera m_camera;
        std::vector<wp_Mesh> m_meshes;

    public:
        // Iterator mumbo-jumbo so that we can walk the mesh list.
        class MeshIterator;

        friend class MeshIterator;
        typedef MeshIterator iterator;
        typedef std::ptrdiff_t difference_type;
        typedef std::size_t size_type;
        typedef std::weak_ptr<Mesh> value_type;
        typedef value_type* pointer;
        typedef value_type& reference;

        iterator begin() const;
        iterator end() const;

        class MeshIterator {
        public:
            MeshIterator(const Scene &scene, unsigned int init_loc);

            bool operator==(const MeshIterator &other) const;
            bool operator!=(const MeshIterator &other) const;
            Scene::value_type operator*();
            MeshIterator &operator++();   // prefix
            MeshIterator operator++(int); // postfix

        private:
            MeshIterator();

            const Scene &m_scene;
            unsigned int m_loc;
        };
    };

    typedef std::unique_ptr<Scene> up_Scene;
    typedef std::shared_ptr<Scene> sp_Scene;
    typedef std::weak_ptr<Scene> wp_Scene;
};

#endif
