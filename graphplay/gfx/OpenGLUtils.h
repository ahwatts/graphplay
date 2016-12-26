// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_GFX_OPENGL_UTILS_H_
#define _GRAPHPLAY_GRAPHPLAY_GFX_OPENGL_UTILS_H_

#include "../graphplay.h"

#include <map>
#include <string>
#include <vector>
#include "../opengl.h"

namespace graphplay {
    namespace gfx {
        typedef std::map<std::string, GLuint> IndexMap;

        GLuint duplicateBuffer(GLenum target, GLuint src);
        GLuint duplicateVertexArrayObject(GLuint src);
        GLuint createAndCompileShader(GLenum shader_type, const char* shader_src);
        GLuint createProgramFromShaders(GLuint vertex_shader, GLuint fragment_shader);
        void getAttachedShaders(GLuint program, std::vector<GLuint> &shaders);
        void getAttributeInfo(GLuint program, IndexMap &attributes);
        void getUniformInfo(GLuint program, IndexMap &uniforms);
        void getUniformBlockInfo(GLuint program, IndexMap &uniform_blocks);

        int sizeOfGLType(GLenum type);
        std::string translateGLType(GLenum type);
        void dumpOpenGLState();

        void printOpenGLError();
    }
}

#endif
