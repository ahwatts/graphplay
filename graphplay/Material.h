// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_MATERIAL_H_
#define _GRAPHPLAY_GRAPHPLAY_MATERIAL_H_

#include <GL/glew.h>
#include <memory>

namespace graphplay {
    class Material
    {
    public:
        Material();
        virtual ~Material();

        virtual void createProgram() = 0;
        void destroyProgram();

        GLuint getVertexShader() const;
        GLuint getFragmentShader() const;
        GLuint getProgram() const;

        virtual GLuint getPositionLocation() const { return m_max_vertex_attribs; }
        virtual GLuint getNormalLocation() const { return m_max_vertex_attribs; }
        virtual GLuint getColorLocation() const { return m_max_vertex_attribs; }
        virtual GLuint getTexCoordLocation() const { return m_max_vertex_attribs; }

        virtual GLuint getProjectionLocation() const { return m_max_vertex_attribs; }
        virtual GLuint getModelViewLocation() const { return m_max_vertex_attribs; }

        GLuint getMaxVertexAttribs() const { return m_max_vertex_attribs; }

    protected:
        GLuint m_program;
        GLuint m_max_vertex_attribs;
    };

    typedef std::unique_ptr<Material> up_Material;
    typedef std::shared_ptr<Material> sp_Material;
    typedef std::weak_ptr<Material> wp_Material;

    class GouraudMaterial : public Material {
    public:
        GouraudMaterial();
        virtual ~GouraudMaterial();

        virtual void createProgram();

        virtual GLuint getPositionLocation() const { return m_position_loc; }
        virtual GLuint getColorLocation() const { return m_color_loc; }

        virtual GLuint getProjectionLocation() const { return m_projection_loc; }
        virtual GLuint getModelViewLocation() const { return m_model_view_loc; }

    protected:
        GLuint m_position_loc;
        GLuint m_color_loc;

        GLuint m_projection_loc;
        GLuint m_model_view_loc;

        static const char *vertex_shader_src, *fragment_shader_src;
    };
};

#endif
