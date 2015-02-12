// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <iostream>
#include <vector>

#include "OpenGLUtils.h"

namespace graphplay {
    GLuint duplicateBuffer(GLenum target, GLuint src) {
        if (glIsBuffer(src)) {
            // Retrieve the size, usage, and data of the buffer.
            GLint size = 0, usage = 0;
            glBindBuffer(target, src);
            glGetBufferParameteriv(target, GL_BUFFER_SIZE, &size);
            glGetBufferParameteriv(target, GL_BUFFER_USAGE, &usage);
            char *src_data = (char *)glMapBuffer(target, GL_READ_ONLY);
            std::vector<char> inter_data(src_data, src_data + size);
            glUnmapBuffer(target);
            glBindBuffer(target, 0);

            // Copy the data to a new buffer.
            GLuint dst = 0;
            glGenBuffers(1, &dst);
            glBindBuffer(target, dst);
            glBufferData(target, size, inter_data.data(), usage);
            glBindBuffer(target, 0);

            return dst;
        }
        else {
            return 0;
        }
    }

    struct VAPState {
        GLuint enabled;
        GLuint array_buffer_binding;
        GLuint size;
        GLuint stride;
        GLenum type;
        GLuint is_normalized;
        GLuint is_integer;
        GLuint divisor;
        GLvoid *offset;
    };

    GLuint duplicateVertexArrayObject(GLuint src) {
        if (glIsVertexArray(src)) {
            glBindVertexArray(src);

            GLint elem_binding = 0, max_attribs = 0;
            glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elem_binding);
            glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);

            VAPState *attribs = new VAPState[max_attribs];
            for (auto i = 0; i < max_attribs; ++i) {
                glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &attribs[i].enabled);
                if (attribs[i].enabled) {
                    glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &attribs[i].array_buffer_binding);
                    glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_SIZE, &attribs[i].size);
                    glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &attribs[i].stride);
                    glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_TYPE, &attribs[i].type);
                    glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &attribs[i].is_normalized);
                    glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_INTEGER, &attribs[i].is_integer);
                    glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_DIVISOR, &attribs[i].divisor);
                    glGetVertexAttribPointerv(i, GL_VERTEX_ATTRIB_ARRAY_POINTER, &attribs[i].offset);
                }
            }

            glBindVertexArray(0);

            GLuint dst = 0;
            glGenVertexArrays(1, &dst);
            glBindVertexArray(dst);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elem_binding);
            for (auto i = 0; i < max_attribs; ++i) {
                if (attribs[i].enabled) {
                    glEnableVertexAttribArray(i);
                    glBindBuffer(GL_ARRAY_BUFFER, attribs[i].array_buffer_binding);
                    glVertexAttribDivisor(i, attribs[i].divisor);

                    switch (attribs[i].type) {
                    case GL_BYTE:
                    case GL_UNSIGNED_BYTE:
                    case GL_SHORT:
                    case GL_UNSIGNED_SHORT:
                    case GL_INT:
                    case GL_UNSIGNED_INT:
                        if (attribs[i].is_integer) {
                            glVertexAttribIPointer(i,
                                attribs[i].size,
                                attribs[i].type,
                                attribs[i].stride,
                                attribs[i].offset);
                        }
                        else {
                            glVertexAttribPointer(i,
                                attribs[i].size,
                                attribs[i].type,
                                attribs[i].is_normalized,
                                attribs[i].stride,
                                attribs[i].offset);
                        }
                        break;
                    case GL_DOUBLE:
                        glVertexAttribLPointer(i,
                            attribs[i].size,
                            attribs[i].type,
                            attribs[i].stride,
                            attribs[i].offset);
                        break;
                    default:
                        glVertexAttribPointer(i,
                            attribs[i].size,
                            attribs[i].type,
                            attribs[i].is_normalized,
                            attribs[i].stride,
                            attribs[i].offset);
                        break;
                    }
                }
            }

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            delete[] attribs;
            return dst;
        }
        else {
            return 0;
        }
    }

    GLuint createAndCompileShader(GLenum shader_type, const char* shader_src) {
        GLuint shader = glCreateShader(shader_type);
        GLint errlen, status, src_length = std::strlen(shader_src);

        glShaderSource(shader, 1, &shader_src, &src_length);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (!status) {
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errlen);
            char *err = new char[errlen];
            glGetShaderInfoLog(shader, errlen, NULL, err);
            std::cerr << "Could not compile shader!" << std::endl;
            std::cerr << "  error: " << err << std::endl;
            std::cerr << "  source:" << std::endl << shader_src << std::endl;
            delete[] err;
            std::exit(1);
        }

        return shader;
    }

    GLuint createProgramFromShaders(GLuint vertex_shader, GLuint fragment_shader) {
        GLuint program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);

        GLint status;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if (!status) {
            GLint errlen;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &errlen);

            char *err = new char[errlen];
            glGetProgramInfoLog(program, errlen, NULL, err);
            std::cerr << "Could not link shader program: " << err << std::endl;
            delete[] err;

            std::exit(1);
        }

        return program;
    }

    void getAttachedShaders(GLuint program, std::vector<GLuint> &shaders) {
        int num_shaders;
        GLuint *shader_return;
        glGetProgramiv(program, GL_ATTACHED_SHADERS, &num_shaders);

        shader_return = new GLuint[num_shaders];
        glGetAttachedShaders(program, num_shaders, NULL, shader_return);

        shaders.clear();
        for (int i = 0; i < num_shaders; ++i) {
            shaders.push_back(shader_return[i]);
        }

        delete[] shader_return;
    }

    void getAttributeInfo(GLuint program, IndexMap &attributes) {
        GLint num_attrs, max_name_len;
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &num_attrs);
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_name_len);

        char *name = new char[max_name_len];

        for (auto i = 0; i < num_attrs; ++i) {
            GLsizei name_len = 0, size = 0;
            GLenum type = 0;
            glGetActiveAttrib(program, i, max_name_len, &name_len, &size, &type, name);
            attributes[name] = i;
        }

        delete[] name;
    }

    void getUniformInfo(GLuint program, IndexMap &uniforms) {
        GLint num_unifs, max_name_len;
        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &num_unifs);
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);

        char *name = new char[max_name_len];

        for (auto i = 0; i < num_unifs; ++i) {
            GLsizei name_len = 0, size = 0;
            GLenum type = 0;
            glGetActiveUniform(program, i, max_name_len, &name_len, &size, &type, name);
            uniforms[name] = i;
        }

        delete[] name;
    }

    void getUniformBlockInfo(GLuint program, IndexMap &uniform_blocks) {
        GLint num_unifbs, max_name_len;
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &num_unifbs);
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &max_name_len);

        char *name = new char[max_name_len];

        for (auto i = 0; i < num_unifbs; ++i) {
            GLsizei name_len = 0;
            glGetActiveUniformBlockName(program, i, max_name_len, &name_len, name);
            uniform_blocks[name] = i;
        }

        delete[] name;
    }
}
