// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _BASIC_SHADER_H_
#define _BASIC_SHADER_H_

#include <GL/glew.h>

class BasicShader
{
public:
    BasicShader(void);
    ~BasicShader(void);

    inline GLuint getVertexShader()   { return m_vertex_shader;   };
    inline GLuint getFragmentShader() { return m_fragment_shader; };
    inline GLuint getProgram()        { return m_shader_program;  };

    inline GLuint getPositionLocation()   { return m_position_loc;   };
    inline GLuint getColorLocation()      { return m_color_loc;      };
    inline GLuint getProjectionLocation() { return m_projection_loc; };
    inline GLuint getModelViewLocation()  { return m_model_view_loc; };

private:
    GLuint m_vertex_shader;
    GLuint m_fragment_shader;
    GLuint m_shader_program;

    GLuint m_position_loc;
    GLuint m_color_loc;
    GLuint m_projection_loc;
    GLuint m_model_view_loc;

    static const char *vertex_shader_src;
    static const char *fragment_shader_src;

    GLuint createAndCompileShader(GLenum shader_type, const char* shader_src);
};

#endif
