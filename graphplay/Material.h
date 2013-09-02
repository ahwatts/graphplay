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

        virtual GLint getPositionLocation() const { return -1; }
        virtual GLint getNormalLocation() const { return -1; }
        virtual GLint getColorLocation() const { return -1; }
        virtual GLint getTexCoordLocation() const { return -1; }

        virtual GLint getProjectionLocation() const { return -1; }
        virtual GLint getModelViewLocation() const { return -1; }
        virtual GLint getModelViewInverseLocation() const { return -1; }

    protected:
        GLuint m_program;
    };

    typedef std::unique_ptr<Material> up_Material;
    typedef std::shared_ptr<Material> sp_Material;
    typedef std::weak_ptr<Material> wp_Material;

    class LambertMaterial : public Material {
    public:
        LambertMaterial();
        virtual ~LambertMaterial();

        virtual void createProgram();

        virtual GLint getPositionLocation() const { return (GLint)m_position_loc; }
        virtual GLint getNormalLocation() const { return (GLint)m_normal_loc; }
        virtual GLint getColorLocation() const { return (GLint)m_color_loc; }

        virtual GLint getProjectionLocation() const { return m_projection_loc; }
        virtual GLint getModelViewLocation() const { return m_model_view_loc; }
        virtual GLint getModelViewInverseLocation() const { return m_model_view_inv_loc; }

    protected:
        GLuint m_position_loc;
        GLuint m_normal_loc;
        GLuint m_color_loc;

        GLint m_projection_loc;
        GLint m_model_view_loc;
        GLint m_model_view_inv_loc;

        static const char *vertex_shader_src, *fragment_shader_src;
    };

    /*
    class PhongMaterial : public Material {
    public:
        PhongMaterial();
        virtual ~PhongMaterial();

        virtual void createProgram();

        virtual GLint getPositionLocation() const { return (GLint)m_position_loc; }
        virtual GLint getNormalLocation() const { return (GLint)m_normal_loc; }

        virtual GLint getProjectionLocation() const { return m_projection_loc; }
        virtual GLint getModelViewLocation() const { return m_model_view_loc; }

    protected:
        GLuint m_position_loc;
        GLuint m_normal_loc;

        GLint m_projection_loc;
        GLint m_model_view_loc;

        static const char *vertex_shader_src, *fragment_shader_src;
    };
    */
};

#endif
