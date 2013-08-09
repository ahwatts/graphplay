// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_MATERIAL_H_
#define _GRAPHPLAY_GRAPHPLAY_MATERIAL_H_

#include <GL/glew.h>

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

        virtual GLuint getPositionLocation() const = 0;
        virtual GLuint getNormalLocation() const = 0;
        virtual GLuint getColorLocation() const = 0;
        virtual GLuint getTexCoordLocation() const = 0;

        virtual GLuint getProjectionLocation() const = 0;
        virtual GLuint getModelViewLocation() const = 0;

    protected:
        GLuint m_program;
    };

    class GouraudMaterial : public Material {
    public:
        GouraudMaterial();
        virtual ~GouraudMaterial();

        virtual GLuint getPositionLocation() const;
        virtual GLuint getNormalLocation() const;
        virtual GLuint getColorLocation() const;
        virtual GLuint getTexCoordLocation() const;

        virtual GLuint getProjectionLocation() const;
        virtual GLuint getModelViewLocation() const;
    private:
        GLuint m_position_loc;
        GLuint m_color_loc;

        GLuint m_projection_loc;
        GLuint m_model_view_loc;

        static const char *vertex_shader_src, *fragment_shader_src;

        GLuint createAndCompileShader(GLenum shader_type, const char* shader_src);
    };
};

#endif
