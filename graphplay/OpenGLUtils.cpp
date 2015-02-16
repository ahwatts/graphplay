// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <cstring>
#include <iostream>
#include <sstream>
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

    std::string translateGLType(GLenum type) {
        std::string rv;

        switch (type) {
        case GL_FLOAT:        rv = "float"; break;
        case GL_FLOAT_VEC2:   rv = "float vec2"; break;
        case GL_FLOAT_VEC3:   rv = "float vec3"; break;
        case GL_FLOAT_VEC4:   rv = "float vec4"; break;
        case GL_FLOAT_MAT2:   rv = "float mat2x2"; break;
        case GL_FLOAT_MAT2x3: rv = "float mat2x3"; break;
        case GL_FLOAT_MAT2x4: rv = "float mat2x4"; break;
        case GL_FLOAT_MAT3x2: rv = "float mat3x2"; break;
        case GL_FLOAT_MAT3:   rv = "float mat3x3"; break;
        case GL_FLOAT_MAT3x4: rv = "float mat3x4"; break;
        case GL_FLOAT_MAT4x2: rv = "float mat4x2"; break;
        case GL_FLOAT_MAT4x3: rv = "float mat4x3"; break;
        case GL_FLOAT_MAT4:   rv = "float mat4x4"; break;

        case GL_INT:        rv = "int"; break;
        case GL_INT_VEC2:   rv = "int vec2"; break;
        case GL_INT_VEC3:   rv = "int vec3"; break;
        case GL_INT_VEC4:   rv = "int vec4"; break;

        case GL_UNSIGNED_INT:        rv = "unsigned int"; break;
        case GL_UNSIGNED_INT_VEC2:   rv = "unsigned int vec2"; break;
        case GL_UNSIGNED_INT_VEC3:   rv = "unsigned int vec3"; break;
        case GL_UNSIGNED_INT_VEC4:   rv = "unsigned int vec4"; break;

        case GL_DOUBLE:        rv = "double"; break;
        case GL_DOUBLE_VEC2:   rv = "double vec2"; break;
        case GL_DOUBLE_VEC3:   rv = "double vec3"; break;
        case GL_DOUBLE_VEC4:   rv = "double vec4"; break;
        case GL_DOUBLE_MAT2:   rv = "double mat2x2"; break;
        case GL_DOUBLE_MAT2x3: rv = "double mat2x3"; break;
        case GL_DOUBLE_MAT2x4: rv = "double mat2x4"; break;
        case GL_DOUBLE_MAT3x2: rv = "double mat3x2"; break;
        case GL_DOUBLE_MAT3:   rv = "double mat3x3"; break;
        case GL_DOUBLE_MAT3x4: rv = "double mat3x4"; break;
        case GL_DOUBLE_MAT4x2: rv = "double mat4x2"; break;
        case GL_DOUBLE_MAT4x3: rv = "double mat4x3"; break;
        case GL_DOUBLE_MAT4:   rv = "double mat4x4"; break;

        default:
            rv = "Unknown OpenGL type";
        }

        return rv;
    }

    int sizeOfGLType(GLenum type) {
        int rv;

        switch (type) {
        case GL_FLOAT:        rv =  1*sizeof(float); break;
        case GL_FLOAT_VEC2:   rv =  2*sizeof(float); break;
        case GL_FLOAT_VEC3:   rv =  3*sizeof(float); break;
        case GL_FLOAT_VEC4:   rv =  4*sizeof(float); break;
        case GL_FLOAT_MAT2:   rv =  4*sizeof(float); break;
        case GL_FLOAT_MAT2x3: rv =  6*sizeof(float); break;
        case GL_FLOAT_MAT2x4: rv =  8*sizeof(float); break;
        case GL_FLOAT_MAT3x2: rv =  6*sizeof(float); break;
        case GL_FLOAT_MAT3:   rv =  9*sizeof(float); break;
        case GL_FLOAT_MAT3x4: rv = 12*sizeof(float); break;
        case GL_FLOAT_MAT4x2: rv =  8*sizeof(float); break;
        case GL_FLOAT_MAT4x3: rv = 12*sizeof(float); break;
        case GL_FLOAT_MAT4:   rv = 16*sizeof(float); break;

        case GL_INT:        rv =  1*sizeof(int); break;
        case GL_INT_VEC2:   rv =  2*sizeof(int); break;
        case GL_INT_VEC3:   rv =  3*sizeof(int); break;
        case GL_INT_VEC4:   rv =  4*sizeof(int); break;

        case GL_UNSIGNED_INT:        rv =  1*sizeof(unsigned int); break;
        case GL_UNSIGNED_INT_VEC2:   rv =  2*sizeof(unsigned int); break;
        case GL_UNSIGNED_INT_VEC3:   rv =  3*sizeof(unsigned int); break;
        case GL_UNSIGNED_INT_VEC4:   rv =  4*sizeof(unsigned int); break;

        case GL_DOUBLE:        rv =  1*sizeof(double); break;
        case GL_DOUBLE_VEC2:   rv =  2*sizeof(double); break;
        case GL_DOUBLE_VEC3:   rv =  3*sizeof(double); break;
        case GL_DOUBLE_VEC4:   rv =  4*sizeof(double); break;
        case GL_DOUBLE_MAT2:   rv =  4*sizeof(double); break;
        case GL_DOUBLE_MAT2x3: rv =  6*sizeof(double); break;
        case GL_DOUBLE_MAT2x4: rv =  8*sizeof(double); break;
        case GL_DOUBLE_MAT3x2: rv =  6*sizeof(double); break;
        case GL_DOUBLE_MAT3:   rv =  9*sizeof(double); break;
        case GL_DOUBLE_MAT3x4: rv = 12*sizeof(double); break;
        case GL_DOUBLE_MAT4x2: rv =  8*sizeof(double); break;
        case GL_DOUBLE_MAT4x3: rv = 12*sizeof(double); break;
        case GL_DOUBLE_MAT4:   rv = 16*sizeof(double); break;

        default:
            rv = 0;
        }

        return rv;
    }

    std::string getUniformValue(GLuint program, GLint location) {
        std::stringstream out;
        GLint size = 0, total_size = 0;
        GLenum type = 0, error = GL_NO_ERROR;
        glGetActiveUniform(program, location, 0, nullptr, &size, &type, nullptr);
        total_size = size*sizeOfGLType(type);
        char *buffer = new char[total_size];

        switch (type) {
        case GL_FLOAT:
        case GL_FLOAT_VEC2:
        case GL_FLOAT_VEC3:
        case GL_FLOAT_VEC4:
        case GL_FLOAT_MAT2:
        case GL_FLOAT_MAT2x3:
        case GL_FLOAT_MAT2x4:
        case GL_FLOAT_MAT3x2:
        case GL_FLOAT_MAT3:
        case GL_FLOAT_MAT3x4:
        case GL_FLOAT_MAT4x2:
        case GL_FLOAT_MAT4x3:
        case GL_FLOAT_MAT4:
            glGetUniformfv(program, location, (float*)buffer);
            error = glGetError();
            if (error == GL_NO_ERROR) {
                out << "[ ";
                for (auto i = 0; i < total_size / sizeof(float); ++i) {
                    out << ((float*)buffer)[i] << ", ";
                }
                out << "]";
            } else {
                out << "Error: " << error;
            }
            break;

        case GL_INT:
        case GL_INT_VEC2:
        case GL_INT_VEC3:
        case GL_INT_VEC4:
            glGetUniformiv(program, location, (int*)buffer);
            error = glGetError();
            if (error == GL_NO_ERROR) {
                out << "[ ";
                for (auto i = 0; i < total_size / sizeof(int); ++i) {
                    out << ((int*)buffer)[i] << ", ";
                }
                out << "]";
            } else {
                out << "Error: " << error;
            }
            break;

        case GL_UNSIGNED_INT:
        case GL_UNSIGNED_INT_VEC2:
        case GL_UNSIGNED_INT_VEC3:
        case GL_UNSIGNED_INT_VEC4:
            glGetUniformuiv(program, location, (unsigned int*)buffer);
            error = glGetError();
            if (error == GL_NO_ERROR) {
                out << "[ ";
                for (auto i = 0; i < total_size / sizeof(unsigned int); ++i) {
                    out << ((unsigned int*)buffer)[i] << ", ";
                }
                out << "]";
            } else {
                out << "Error: " << error;
            }
            break;

        case GL_DOUBLE:
        case GL_DOUBLE_VEC2:
        case GL_DOUBLE_VEC3:
        case GL_DOUBLE_VEC4:
        case GL_DOUBLE_MAT2:
        case GL_DOUBLE_MAT2x3:
        case GL_DOUBLE_MAT2x4:
        case GL_DOUBLE_MAT3x2:
        case GL_DOUBLE_MAT3:
        case GL_DOUBLE_MAT3x4:
        case GL_DOUBLE_MAT4x2:
        case GL_DOUBLE_MAT4x3:
        case GL_DOUBLE_MAT4:
            glGetUniformdv(program, location, (double*)buffer);
            error = glGetError();
            if (error == GL_NO_ERROR) {
                out << "[ ";
                for (auto i = 0; i < total_size / sizeof(double); ++i) {
                    out << ((double*)buffer)[i] << ", ";
                }
                out << "]";
            } else {
                out << "Error: " << error;
            }
            break;

        default:
            out << "Unknown OpenGL Type: " << type;
        }

        delete [] buffer;
        return out.str();
    }

    void dumpOpenGLState() {
        GLint progid = 0;
        GLint num_things = 0, max_name_len = 0, name_len = 0, size = 0, element_array_buffer = 0;
        GLenum type = 0;
        char *name = nullptr;

        glGetIntegerv(GL_CURRENT_PROGRAM, &progid);
        std::cout << "OpenGL State:" << std::endl;
        std::cout << "  Current program: " << progid << std::endl;

        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &element_array_buffer);
        std::cout << "    Element array buffer: " << element_array_buffer << std::endl;

        glGetProgramiv(progid, GL_ACTIVE_ATTRIBUTES, &num_things);
        glGetProgramiv(progid, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_name_len);
        name = new char[max_name_len];
        std::cout << "    Attributes: " << num_things << std::endl;
        for (auto i = 0; i < num_things; ++i) {
            glGetActiveAttrib(progid, i, max_name_len, &name_len, &size, &type, name);
            std::cout << "      " << i << ": " << name << ": type: " << translateGLType(type) << " size: " << size << std::endl;

            VAPState array_state;
            glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &array_state.enabled);
            if (array_state.enabled != GL_FALSE) {
                glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &array_state.array_buffer_binding);
                glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_SIZE, &array_state.size);
                glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &array_state.stride);
                glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_TYPE, &array_state.type);
                glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &array_state.is_normalized);
                glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_INTEGER, &array_state.is_integer);
                glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_DIVISOR, &array_state.divisor);
                glGetVertexAttribPointerv(i, GL_VERTEX_ATTRIB_ARRAY_POINTER, &array_state.offset);
                std::cout << "         "
                          << "array value: enabled"
                          << " buffer binding: " << array_state.array_buffer_binding
                          << " size: " << array_state.size
                          << " stride: " << array_state.stride
                          << " type: " << translateGLType(array_state.type)
                          << " normalized: " << array_state.is_normalized
                          << " integral: " << array_state.is_integer
                          << " divisor: " << array_state.divisor
                          << " offset: " << (long)array_state.offset
                          << std::endl;
            } else {
                std::cout << "         array: disabled" << std::endl;
            }
        }
        delete [] name;

        glGetProgramiv(progid, GL_ACTIVE_UNIFORMS, &num_things);
        glGetProgramiv(progid, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);
        name = new char[max_name_len];
        std::cout << "    Uniforms: " << num_things << std::endl;
        for (GLuint i = 0; i < num_things; ++i) {
            GLint block_index = 0, itype = 0;
            glGetActiveUniformsiv(progid, 1, &i, GL_UNIFORM_BLOCK_INDEX, &block_index);
            glGetActiveUniformName(progid, i, max_name_len, nullptr, name);
            if (block_index == -1) {
                glGetActiveUniformsiv(progid, 1, &i, GL_UNIFORM_SIZE, &size);
                glGetActiveUniformsiv(progid, 1, &i, GL_UNIFORM_TYPE, &itype);
                std::cout << "      " << i << ": " << name << ": type: " << translateGLType((GLenum)itype) << " size: " << size << std::endl;
                std::cout << "         value: " << getUniformValue(progid, i) << std::endl;
            } else {
                std::cout << "      " << i << ": (in block)" << std::endl;
            }
        }
        delete [] name;

        glGetProgramiv(progid, GL_ACTIVE_UNIFORM_BLOCKS, &num_things);
        glGetProgramiv(progid, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &max_name_len);
        name = new char[max_name_len];
        std::cout << "    Uniform blocks: " << num_things << std::endl;
        for (GLuint i = 0; i < num_things; ++i) {
            GLint binding = -1, bound_buffer = -1;
            glGetActiveUniformBlockiv(progid, i, GL_UNIFORM_BLOCK_BINDING, &binding);
            glGetActiveUniformBlockName(progid, i, max_name_len, nullptr, name);
            glGetIntegeri_v(GL_UNIFORM_BUFFER_BINDING, binding, &bound_buffer);
            std::cout << "      " << i << ": " << name << ": binding: " << binding << " buffer: " << bound_buffer << std::endl;
        }
        delete [] name;

        std::cout << std::endl;
    }
}
