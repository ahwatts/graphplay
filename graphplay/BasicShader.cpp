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
    char *err;

    vertex_shader = createAndCompileShader(GL_VERTEX_SHADER, vertex_shader_src);
    fragment_shader = createAndCompileShader(GL_FRAGMENT_SHADER, fragment_shader_src);
    shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &status);
    if (!status) {
        glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &errlen);
        err = (char*)malloc(errlen * sizeof(char));
        glGetProgramInfoLog(shader_program, errlen, NULL, err);
        fprintf(stderr, "Could not link shader program: %s\n", err);
        free(err);
        exit(1);
    }
}

BasicShader::~BasicShader(void) {
    if (glIsShader(vertex_shader) == GL_TRUE) {
        glDeleteShader(vertex_shader);
    }

    if (glIsShader(fragment_shader) == GL_TRUE) {
        glDeleteShader(fragment_shader);
    }

    if (glIsProgram(shader_program) == GL_TRUE) {
        glDeleteProgram(shader_program);
    }
}

GLuint BasicShader::createAndCompileShader(GLenum shader_type, const char* shader_src) {
    GLuint shader = glCreateShader(shader_type);
    GLint errlen, status, src_length = strlen(shader_src);
    char* err;

    glShaderSource(shader, 1, &shader_src, &src_length);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errlen);
        err = (char*)malloc(errlen * sizeof(char));
        glGetShaderInfoLog(shader, errlen - 1, NULL, err);
        fprintf(stderr, "Could not compile shader!\nsource: %s\nerror: %s\n", shader_src, err);
        free(err);
        exit(1);
    }

    return shader;
}
