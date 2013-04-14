// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BasicShader.h"

const char* BasicShader::vertex_shader_src =
    "attribute vec3 aPosition;"
    "attribute vec4 aColor;"
    "uniform mat4 uModelView;"
    "uniform mat4 uProjection;"
    "varying vec4 vColor;"
    "void main(void) {"
    "    gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);"
    "    vColor = aColor;"
    "}";

const char* BasicShader::fragment_shader_src =
    "varying vec4 vColor;"
    "void main(void) {"
    "    gl_FragColor = vColor;"
    "}";

BasicShader::BasicShader(void) {
    GLint status, errlen;

    m_vertex_shader = createAndCompileShader(GL_VERTEX_SHADER, vertex_shader_src);
    m_fragment_shader = createAndCompileShader(GL_FRAGMENT_SHADER, fragment_shader_src);
    m_shader_program = glCreateProgram();

    glAttachShader(m_shader_program, m_vertex_shader);
    glAttachShader(m_shader_program, m_fragment_shader);
    glLinkProgram(m_shader_program);
    glGetProgramiv(m_shader_program, GL_LINK_STATUS, &status);
    if (!status) {
        char *err;
        glGetProgramiv(m_shader_program, GL_INFO_LOG_LENGTH, &errlen);
        err = (char*)malloc(errlen * sizeof(char));
        glGetProgramInfoLog(m_shader_program, errlen, NULL, err);
        fprintf(stderr, "Could not link shader program: %s\n", err);
        free(err);
        exit(1);
    }

    m_position_loc = glGetAttribLocation(m_shader_program, "aPosition");
    m_color_loc = glGetAttribLocation(m_shader_program, "aColor");
    m_projection_loc = glGetUniformLocation(m_shader_program, "uProjection");
    m_model_view_loc = glGetUniformLocation(m_shader_program, "uModelView");
}

BasicShader::~BasicShader(void) {
    if (glIsShader(m_vertex_shader) == GL_TRUE) {
        glDeleteShader(m_vertex_shader);
    }

    if (glIsShader(m_fragment_shader) == GL_TRUE) {
        glDeleteShader(m_fragment_shader);
    }

    if (glIsProgram(m_shader_program) == GL_TRUE) {
        glDeleteProgram(m_shader_program);
    }
}

GLuint BasicShader::createAndCompileShader(GLenum shader_type, const char* shader_src) {
    GLuint shader = glCreateShader(shader_type);
    GLint errlen, status, src_length = strlen(shader_src);

    glShaderSource(shader, 1, &shader_src, &src_length);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        char *err;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errlen);
        err = (char*)malloc(errlen * sizeof(char));
        glGetShaderInfoLog(shader, errlen - 1, NULL, err);
        fprintf(stderr, "Could not compile shader!\nsource: %s\nerror: %s\n", shader_src, err);
        free(err);
        exit(1);
    }

    return shader;
}
