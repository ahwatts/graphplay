// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_OPENGL_UTILS_H_
#define _GRAPHPLAY_GRAPHPLAY_OPENGL_UTILS_H_

#include <map>

#include "opengl.h"

namespace graphplay {
    typedef std::map<std::string, GLuint> IndexMap;

    GLuint duplicateBuffer(GLenum target, GLuint src);
    GLuint duplicateVertexArrayObject(GLuint src);
    GLuint createAndCompileShader(GLenum shader_type, const char* shader_src);
    GLuint createProgramFromShaders(GLuint vertex_shader, GLuint fragment_shader);
    void getAttachedShaders(GLuint program, std::vector<GLuint> &shaders);
    void getAttributeInfo(GLuint program, IndexMap &attributes);
    void getUniformInfo(GLuint program, IndexMap &uniforms);
    void getUniformBlockInfo(GLuint program, IndexMap &uniform_blocks);
}

#endif
