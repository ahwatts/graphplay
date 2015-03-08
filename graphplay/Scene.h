// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_SCENE_H_
#define _GRAPHPLAY_GRAPHPLAY_SCENE_H_

#include "graphplay.h"

#include <memory>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "opengl.h"

#include "Camera.h"
#include "Mesh.h"

namespace graphplay {
#ifdef MSVC
    const
#else
    constexpr
#endif
    int MAX_LIGHTS = 10;

    struct Light {
        bool enabled;
        glm::vec3 position;
        glm::vec4 color;
        int specular_exp;
    };

    class Scene
    {
    public:
        typedef std::unique_ptr<Scene> uptr_type;
        typedef std::shared_ptr<Scene> sptr_type;
        typedef std::weak_ptr<Scene> wptr_type;
        typedef std::vector<Mesh::wptr_type> mesh_list_type;

        Scene(unsigned int vp_width, unsigned int vp_height);
        Scene(const Scene &other);
        Scene(Scene &&other);
        ~Scene();

        Scene& operator=(const Scene &other);
        Scene& operator=(Scene &&other);

        // Update the projection transformation for the new viewport.
        void setViewport(unsigned int new_width, unsigned int new_height);
        inline unsigned int getViewportWidth() const { return m_vp_width; }
        inline unsigned int getViewportHeight() const { return m_vp_height; }

        // Manage the list of meshes.
        void addMesh(Mesh::wptr_type mesh);
        Mesh::wptr_type removeMesh(Mesh::wptr_type mesh);

        // Manipulate the camera.
        inline Camera &getCamera() { return m_camera; }

        // Manage the uniform buffers.
        void createBuffers();
        void updateBuffers();
        void bindBuffers();
        void unbindBuffers();
        void deleteBuffers();

        // Draw the scene.
        void render();

    private:
        unsigned int m_vp_width, m_vp_height;

        Camera m_camera;
        glm::mat4x4 m_projection;
        Light m_lights[MAX_LIGHTS];

        mesh_list_type m_meshes;

        GLuint m_view_projection_uniform_buffer;
        GLuint m_light_uniform_buffer;
        IndexMap m_uniform_buffer_bindings;
    };
};

#endif
