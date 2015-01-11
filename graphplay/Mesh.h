// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_MESH_H_
#define _GRAPHPLAY_GRAPHPLAY_MESH_H_

#include <glm/mat4x4.hpp>
#include <memory>

#include "Geometry.h"
#include "Material.h"

namespace graphplay {
    class Mesh
    {
    public:
        Mesh();
        Mesh(sp_Geometry geo, sp_Material mat);

        void setGeometry(sp_Geometry geo);
        void setMaterial(sp_Material mat);

        inline const float* getTransform() const { return m_model_transform; }
        void setTransform(const glm::mat4x4 &new_transform);

        virtual void render(const glm::mat4x4 &projection, const glm::mat4x4 &view) const;

        inline const wp_Geometry getGeometry() const { return wp_Geometry(m_geometry); }
        inline const wp_Material getMaterial() const { return wp_Material(m_material); }

    protected:
        // glm::mat4x4 m_model_transform;
        float m_model_transform[16];
        sp_Geometry m_geometry;
        sp_Material m_material;
    };

    typedef std::unique_ptr<Mesh> up_Mesh;
    typedef std::shared_ptr<Mesh> sp_Mesh;
    typedef std::weak_ptr<Mesh> wp_Mesh;

    class DebugMesh : public Mesh
    {
    public:
        DebugMesh(sp_Geometry geo);

        virtual void render(const glm::mat4x4 &projection, const glm::mat4x4 &view) const;

        void printTransformFeedback() const;

    protected:
        GLuint m_program_1, m_program_2;

        GLuint m_vao_1, m_vao_2;
        GLuint m_feedback_buffer;
        GLuint m_feedback_element_buffer;

        GLuint m_position_loc;
        GLuint m_normal_loc;
        GLuint m_color_loc;

        GLint m_model_loc;
        GLint m_model_3_inv_trans_loc;
        GLint m_view_loc;
        GLint m_projection_loc;
        GLint m_light_position_loc;
        GLint m_light_color_loc;
        GLint m_specular_exponent_loc;

        GLuint m_position_loc_2;
        GLint m_projection_loc_2;

        // The shaders for the first run.
        static const char *vertex_shader_1_src, *fragment_shader_1_src;

        // The shaders for the second run, with the transform feedback.
        static const char *vertex_shader_2_src, *fragment_shader_2_src;
    };
};

#endif
