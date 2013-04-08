#ifndef _BASIC_SHADER_H_
#define _BASIC_SHADER_H_

#include <GL/glew.h>

class BasicShader
{
public:
    BasicShader(void);
    ~BasicShader(void);

    inline GLuint getVertexShader()   { return m_vertex_shader; };
    inline GLuint getFragmentShader() { return m_fragment_shader; };
    inline GLuint getProgram()        { return m_shader_program; };

private:
    GLuint m_vertex_shader;
    GLuint m_fragment_shader;
    GLuint m_shader_program;

    static const char *vertex_shader_src;
    static const char *fragment_shader_src;

    GLuint createAndCompileShader(GLenum shader_type, const char* shader_src);
};

#endif
