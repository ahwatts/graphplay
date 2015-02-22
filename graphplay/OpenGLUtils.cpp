// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <cstring>
#include <iomanip>
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
            GLint location = -1;
            glGetActiveAttrib(program, i, max_name_len, &name_len, &size, &type, name);
            location = glGetAttribLocation(program, name);
            attributes[name] = location;
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
            GLint location = -1;
            glGetActiveUniform(program, i, max_name_len, &name_len, &size, &type, name);
            location = glGetUniformLocation(program, name);
            uniforms[name] = location;
        }

        delete[] name;
    }

    void getUniformBlockInfo(GLuint program, IndexMap &uniform_blocks) {
        GLint num_unifbs, max_name_len;
        GLuint index = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &num_unifbs);
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &max_name_len);

        char *name = new char[max_name_len];

        for (auto i = 0; i < num_unifbs; ++i) {
            GLsizei name_len = 0;
            glGetActiveUniformBlockName(program, i, max_name_len, &name_len, name);
            index = glGetUniformBlockIndex(program, name);
            uniform_blocks[name] = index;
        }

        delete[] name;
    }

    std::string translateGLType(GLenum type) {
        std::stringstream out;

        switch (type) {
        case GL_BOOL:        out << "bool"; break;
        case GL_BOOL_VEC2:   out << "bool vec2"; break;
        case GL_BOOL_VEC3:   out << "bool vec3"; break;
        case GL_BOOL_VEC4:   out << "bool vec4"; break;

        case GL_FLOAT:        out << "float"; break;
        case GL_FLOAT_VEC2:   out << "float vec2"; break;
        case GL_FLOAT_VEC3:   out << "float vec3"; break;
        case GL_FLOAT_VEC4:   out << "float vec4"; break;
        case GL_FLOAT_MAT2:   out << "float mat2x2"; break;
        case GL_FLOAT_MAT2x3: out << "float mat2x3"; break;
        case GL_FLOAT_MAT2x4: out << "float mat2x4"; break;
        case GL_FLOAT_MAT3x2: out << "float mat3x2"; break;
        case GL_FLOAT_MAT3:   out << "float mat3x3"; break;
        case GL_FLOAT_MAT3x4: out << "float mat3x4"; break;
        case GL_FLOAT_MAT4x2: out << "float mat4x2"; break;
        case GL_FLOAT_MAT4x3: out << "float mat4x3"; break;
        case GL_FLOAT_MAT4:   out << "float mat4x4"; break;

        case GL_INT:        out << "int"; break;
        case GL_INT_VEC2:   out << "int vec2"; break;
        case GL_INT_VEC3:   out << "int vec3"; break;
        case GL_INT_VEC4:   out << "int vec4"; break;

        case GL_UNSIGNED_INT:        out << "unsigned int"; break;
        case GL_UNSIGNED_INT_VEC2:   out << "unsigned int vec2"; break;
        case GL_UNSIGNED_INT_VEC3:   out << "unsigned int vec3"; break;
        case GL_UNSIGNED_INT_VEC4:   out << "unsigned int vec4"; break;

        case GL_DOUBLE:        out << "double"; break;
        case GL_DOUBLE_VEC2:   out << "double vec2"; break;
        case GL_DOUBLE_VEC3:   out << "double vec3"; break;
        case GL_DOUBLE_VEC4:   out << "double vec4"; break;
        case GL_DOUBLE_MAT2:   out << "double mat2x2"; break;
        case GL_DOUBLE_MAT2x3: out << "double mat2x3"; break;
        case GL_DOUBLE_MAT2x4: out << "double mat2x4"; break;
        case GL_DOUBLE_MAT3x2: out << "double mat3x2"; break;
        case GL_DOUBLE_MAT3:   out << "double mat3x3"; break;
        case GL_DOUBLE_MAT3x4: out << "double mat3x4"; break;
        case GL_DOUBLE_MAT4x2: out << "double mat4x2"; break;
        case GL_DOUBLE_MAT4x3: out << "double mat4x3"; break;
        case GL_DOUBLE_MAT4:   out << "double mat4x4"; break;

        default:
            out << "Unknown OpenGL type (" << type << ")";
        }

        return out.str();
    }

    int sizeOfGLType(GLenum type) {
        int rv;

        switch (type) {
        case GL_FLOAT:        rv =  1*sizeof(GLfloat); break;
        case GL_FLOAT_VEC2:   rv =  2*sizeof(GLfloat); break;
        case GL_FLOAT_VEC3:   rv =  3*sizeof(GLfloat); break;
        case GL_FLOAT_VEC4:   rv =  4*sizeof(GLfloat); break;
        case GL_FLOAT_MAT2:   rv =  4*sizeof(GLfloat); break;
        case GL_FLOAT_MAT2x3: rv =  6*sizeof(GLfloat); break;
        case GL_FLOAT_MAT2x4: rv =  8*sizeof(GLfloat); break;
        case GL_FLOAT_MAT3x2: rv =  6*sizeof(GLfloat); break;
        case GL_FLOAT_MAT3:   rv =  9*sizeof(GLfloat); break;
        case GL_FLOAT_MAT3x4: rv = 12*sizeof(GLfloat); break;
        case GL_FLOAT_MAT4x2: rv =  8*sizeof(GLfloat); break;
        case GL_FLOAT_MAT4x3: rv = 12*sizeof(GLfloat); break;
        case GL_FLOAT_MAT4:   rv = 16*sizeof(GLfloat); break;

        case GL_INT:        rv =  1*sizeof(GLint); break;
        case GL_INT_VEC2:   rv =  2*sizeof(GLint); break;
        case GL_INT_VEC3:   rv =  3*sizeof(GLint); break;
        case GL_INT_VEC4:   rv =  4*sizeof(GLint); break;

        case GL_UNSIGNED_INT:        rv =  1*sizeof(GLuint); break;
        case GL_UNSIGNED_INT_VEC2:   rv =  2*sizeof(GLuint); break;
        case GL_UNSIGNED_INT_VEC3:   rv =  3*sizeof(GLuint); break;
        case GL_UNSIGNED_INT_VEC4:   rv =  4*sizeof(GLuint); break;

        case GL_DOUBLE:        rv =  1*sizeof(GLdouble); break;
        case GL_DOUBLE_VEC2:   rv =  2*sizeof(GLdouble); break;
        case GL_DOUBLE_VEC3:   rv =  3*sizeof(GLdouble); break;
        case GL_DOUBLE_VEC4:   rv =  4*sizeof(GLdouble); break;
        case GL_DOUBLE_MAT2:   rv =  4*sizeof(GLdouble); break;
        case GL_DOUBLE_MAT2x3: rv =  6*sizeof(GLdouble); break;
        case GL_DOUBLE_MAT2x4: rv =  8*sizeof(GLdouble); break;
        case GL_DOUBLE_MAT3x2: rv =  6*sizeof(GLdouble); break;
        case GL_DOUBLE_MAT3:   rv =  9*sizeof(GLdouble); break;
        case GL_DOUBLE_MAT3x4: rv = 12*sizeof(GLdouble); break;
        case GL_DOUBLE_MAT4x2: rv =  8*sizeof(GLdouble); break;
        case GL_DOUBLE_MAT4x3: rv = 12*sizeof(GLdouble); break;
        case GL_DOUBLE_MAT4:   rv = 16*sizeof(GLdouble); break;

        default:
            rv = 0;
        }

        return rv;
    }

    // std::string getUniformValue(GLuint program, GLint location) {
    //     std::stringstream out;
    //     int total_size = 0;
    //     GLint size = 0;
    //     GLenum type = GL_INVALID_ENUM, error = GL_NO_ERROR;

    //     glGetActiveUniform(program, location, 0, nullptr, &size, &type, nullptr);
    //     total_size = size*sizeOfGLType(type);
    //     char *buffer = new char[total_size];

    //     switch (type) {
    //     case GL_FLOAT:
    //     case GL_FLOAT_VEC2:
    //     case GL_FLOAT_VEC3:
    //     case GL_FLOAT_VEC4:
    //     case GL_FLOAT_MAT2:
    //     case GL_FLOAT_MAT2x3:
    //     case GL_FLOAT_MAT2x4:
    //     case GL_FLOAT_MAT3x2:
    //     case GL_FLOAT_MAT3:
    //     case GL_FLOAT_MAT3x4:
    //     case GL_FLOAT_MAT4x2:
    //     case GL_FLOAT_MAT4x3:
    //     case GL_FLOAT_MAT4:
    //         glGetUniformfv(program, location, (GLfloat*)buffer);
    //         error = glGetError();
    //         if (error == GL_NO_ERROR) {
    //             out << "[ ";
    //             for (int i = 0; i < (int)(total_size / sizeof(GLfloat)); ++i) {
    //                 out << ((GLfloat*)buffer)[i] << ", ";
    //             }
    //             out << "]";
    //         } else {
    //             out << "Error: " << error;
    //         }
    //         break;

    //     case GL_INT:
    //     case GL_INT_VEC2:
    //     case GL_INT_VEC3:
    //     case GL_INT_VEC4:
    //         glGetUniformiv(program, location, (GLint*)buffer);
    //         error = glGetError();
    //         if (error == GL_NO_ERROR) {
    //             out << "[ ";
    //             for (int i = 0; i < (int)(total_size / sizeof(GLint)); ++i) {
    //                 out << ((int*)buffer)[i] << ", ";
    //             }
    //             out << "]";
    //         } else {
    //             out << "Error: " << error;
    //         }
    //         break;

    //     case GL_UNSIGNED_INT:
    //     case GL_UNSIGNED_INT_VEC2:
    //     case GL_UNSIGNED_INT_VEC3:
    //     case GL_UNSIGNED_INT_VEC4:
    //         glGetUniformuiv(program, location, (GLuint*)buffer);
    //         error = glGetError();
    //         if (error == GL_NO_ERROR) {
    //             out << "[ ";
    //             for (int i = 0; i < (int)(total_size / sizeof(GLuint)); ++i) {
    //                 out << ((GLuint*)buffer)[i] << ", ";
    //             }
    //             out << "]";
    //         } else {
    //             out << "Error: " << error;
    //         }
    //         break;

    //     case GL_DOUBLE:
    //     case GL_DOUBLE_VEC2:
    //     case GL_DOUBLE_VEC3:
    //     case GL_DOUBLE_VEC4:
    //     case GL_DOUBLE_MAT2:
    //     case GL_DOUBLE_MAT2x3:
    //     case GL_DOUBLE_MAT2x4:
    //     case GL_DOUBLE_MAT3x2:
    //     case GL_DOUBLE_MAT3:
    //     case GL_DOUBLE_MAT3x4:
    //     case GL_DOUBLE_MAT4x2:
    //     case GL_DOUBLE_MAT4x3:
    //     case GL_DOUBLE_MAT4:
    //         glGetUniformdv(program, location, (GLdouble*)buffer);
    //         error = glGetError();
    //         if (error == GL_NO_ERROR) {
    //             out << "[ ";
    //             for (int i = 0; i < (int)(total_size / sizeof(GLdouble)); ++i) {
    //                 out << ((GLdouble*)buffer)[i] << ", ";
    //             }
    //             out << "]";
    //         } else {
    //             out << "Error: " << error;
    //         }
    //         break;

    //     default:
    //         out << "Unknown OpenGL Type: " << type;
    //     }

    //     delete [] buffer;
    //     return out.str();
    // }

    void dumpProgramAttributes(GLuint progid, const char *prefix) {
        GLint num_attribs = 0, max_name_len = 0, size = 0, location = -1;
        GLenum type = GL_INVALID_ENUM;
        char *name = nullptr;
        const char *sep = " | ";
        VAPState array_state;
        int num_array = 0, num_non_array = 0;

        if (!glIsProgram(progid)) return;

        glGetProgramiv(progid, GL_ACTIVE_ATTRIBUTES, &num_attribs);
        glGetProgramiv(progid, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_name_len);
        name = new char[max_name_len];

        std::cout << prefix << "Attributes (" << num_attribs << "):" << std::endl;

        // Table headers.
        std::cout << std::left << prefix << sep
                  << std::setw(5)            << "Index"    << sep
                  << std::setw(max_name_len) << "Name"     << sep
                  << std::setw(8)            << "Location" << sep
                  << std::setw(17)           << "Type"     << sep
                  << std::setw(4)            << "Size"     << sep
                  << std::endl;

        // Vertical separator.
        std::cout << std::setfill('-');
        std::cout << prefix << sep
                  << std::setw(5)            << "" << sep
                  << std::setw(max_name_len) << "" << sep
                  << std::setw(8)            << "" << sep
                  << std::setw(17)           << "" << sep
                  << std::setw(4)            << "" << sep
                  << std::endl;
        std::cout << std::setfill(' ');

        for (auto i = 0; i < num_attribs; ++i) {
            glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &array_state.enabled);
            glGetActiveAttrib(progid, i, max_name_len, nullptr, &size, &type, name);
            location = glGetAttribLocation(progid, name);

            // Table row.
            std::cout << prefix << sep
                      << std::right << std::setw(5)            << i        << sep
                      << std::left  << std::setw(max_name_len) << name     << sep
                      << std::right << std::setw(8)            << location << sep
                      << std::left  << std::setw(17)           << translateGLType(type) << sep
                      << std::right << std::setw(4)            << size     << sep
                      << std::endl;

            if (array_state.enabled != GL_FALSE) {
                num_array += 1;
            } else {
                num_non_array += 1;
            }
        }

        std::cout << std::endl;

        if (num_array > 0) {
            std::cout << prefix << "Vertex Attribute Array State (" << num_array << " attributes)" << std::endl;

            std::cout << std::left << prefix << sep
                      << std::setw(max_name_len) << "Name"           << sep
                      << std::setw(8)            << "Location"       << sep
                      << std::setw(17)           << "Type"           << sep
                      << std::setw(4)            << "Size"           << sep
                      << std::setw(6)            << "Stride"         << sep
                      << std::setw(6)            << "Offset"         << sep
                      << std::setw(11)           << "Normalized?"    << sep
                      << std::setw(8)            << "Integer?"       << sep
                      << std::setw(7)            << "Divisor"        << sep
                      << std::setw(14)           << "Buffer Binding" << sep
                      << std::endl;

            std::cout << std::setfill('-');
            std::cout << prefix << sep
                << std::setw(max_name_len) << "" << sep
                << std::setw(8)            << "" << sep
                << std::setw(17)           << "" << sep
                << std::setw(4)            << "" << sep
                << std::setw(6)            << "" << sep
                << std::setw(6)            << "" << sep
                << std::setw(11)           << "" << sep
                << std::setw(8)            << "" << sep
                << std::setw(7)            << "" << sep
                << std::setw(14)           << "" << sep
                << std::endl;
            std::cout << std::setfill(' ');

            for (auto i = 0; i < num_attribs; ++i) {
                glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &array_state.enabled);
                glGetActiveAttrib(progid, i, max_name_len, nullptr, &size, &type, name);
                location = glGetAttribLocation(progid, name);

                if (array_state.enabled != GL_FALSE) {
                    glGetVertexAttribIuiv(location, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &array_state.array_buffer_binding);
                    glGetVertexAttribIuiv(location, GL_VERTEX_ATTRIB_ARRAY_SIZE, &array_state.size);
                    glGetVertexAttribIuiv(location, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &array_state.stride);
                    glGetVertexAttribIuiv(location, GL_VERTEX_ATTRIB_ARRAY_TYPE, &array_state.type);
                    glGetVertexAttribIuiv(location, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &array_state.is_normalized);
                    glGetVertexAttribIuiv(location, GL_VERTEX_ATTRIB_ARRAY_INTEGER, &array_state.is_integer);
                    glGetVertexAttribIuiv(location, GL_VERTEX_ATTRIB_ARRAY_DIVISOR, &array_state.divisor);
                    glGetVertexAttribPointerv(location, GL_VERTEX_ATTRIB_ARRAY_POINTER, &array_state.offset);

                    std::cout << std::boolalpha << prefix << sep
                              << std::left  << std::setw(max_name_len) << name                                    << sep
                              << std::right << std::setw(8)            << location                                << sep
                              << std::left  << std::setw(17)           << translateGLType(array_state.type)       << sep
                              << std::right << std::setw(4)            << array_state.size                        << sep
                              << std::right << std::setw(6)            << array_state.stride                      << sep
                              << std::right << std::setw(6)            << (long long)array_state.offset                << sep
                              << std::left  << std::setw(11)           << (array_state.is_normalized != GL_FALSE) << sep
                              << std::left  << std::setw(8)            << (array_state.is_integer != GL_FALSE)    << sep
                              << std::right << std::setw(7)            << array_state.divisor                     << sep
                              << std::right << std::setw(14)           << array_state.array_buffer_binding        << sep
                              << std::endl;
                }
            }
        }

        if (num_non_array > 0) {
            std::cout << prefix << "Vertex Attribute Generic State (" << num_non_array << " attributes)" << std::endl;
        }

        delete [] name;
    }

    void dumpProgramUniforms(GLuint progid, const char *prefix) {
        GLint num_unifs = 0, max_name_len = 0, location = -1;
        char *name = nullptr;
        const char *sep = " | ";
        GLuint *indices = nullptr;
        GLint *types = nullptr, *sizes = nullptr, *block_indices = nullptr, *offsets = nullptr,
            *array_strides = nullptr, *matrix_strides = nullptr, *row_majors = nullptr;
        int in_block = 0, not_in_block = 0;

        glGetProgramiv(progid, GL_ACTIVE_UNIFORMS, &num_unifs);
        glGetProgramiv(progid, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);
        name = new char[max_name_len];
        indices = new GLuint[num_unifs];
        types = new GLint[num_unifs];
        sizes = new GLint[num_unifs];
        block_indices = new GLint[num_unifs];
        offsets = new GLint[num_unifs];
        array_strides = new GLint[num_unifs];
        matrix_strides = new GLint[num_unifs];
        row_majors = new GLint[num_unifs];
        for (auto i = 0; i < num_unifs; ++i) { indices[i] = i; }

        glGetActiveUniformsiv(progid, num_unifs, indices, GL_UNIFORM_TYPE, types);
        glGetActiveUniformsiv(progid, num_unifs, indices, GL_UNIFORM_SIZE, sizes);
        glGetActiveUniformsiv(progid, num_unifs, indices, GL_UNIFORM_BLOCK_INDEX, block_indices);
        glGetActiveUniformsiv(progid, num_unifs, indices, GL_UNIFORM_OFFSET, offsets);
        glGetActiveUniformsiv(progid, num_unifs, indices, GL_UNIFORM_ARRAY_STRIDE, array_strides);
        glGetActiveUniformsiv(progid, num_unifs, indices, GL_UNIFORM_MATRIX_STRIDE, matrix_strides);
        glGetActiveUniformsiv(progid, num_unifs, indices, GL_UNIFORM_IS_ROW_MAJOR, row_majors);

        for (auto i = 0; i < num_unifs; ++i) {
            if (block_indices[i] == -1) {
                not_in_block += 1;
            } else {
                in_block += 1;
            }
        }

        std::cout << prefix << "Uniforms ("
                  << num_unifs << " total, "
                  << not_in_block << " not in a block, "
                  << in_block << " in blocks)"
                  << std::endl;

        std::cout << std::left << prefix << sep
                  << std::setw(5)            << "Index"         << sep
                  << std::setw(max_name_len) << "Name"          << sep
                  << std::setw(8)            << "Location"      << sep
                  << std::setw(11)           << "Block Index"   << sep
                  << std::setw(17)           << "Type"          << sep
                  << std::setw(4)            << "Size"          << sep
                  << std::setw(6)            << "Offset"        << sep
                  << std::setw(12)           << "Array Stride"  << sep
                  << std::setw(13)           << "Matrix Stride" << sep
                  << std::setw(10)           << "Row Major?"    << sep
                  << std::endl;

        std::cout << std::setfill('-');
        std::cout << prefix << sep
                  << std::setw(5)            << "" << sep
                  << std::setw(max_name_len) << "" << sep
                  << std::setw(8)            << "" << sep
                  << std::setw(11)           << "" << sep
                  << std::setw(17)           << "" << sep
                  << std::setw(4)            << "" << sep
                  << std::setw(6)            << "" << sep
                  << std::setw(12)           << "" << sep
                  << std::setw(13)           << "" << sep
                  << std::setw(10)           << "" << sep
                  << std::endl;
        std::cout << std::setfill(' ');

        for (auto i = 0; i < num_unifs; ++i) {
            glGetActiveUniformName(progid, i, max_name_len, nullptr, name);
            location = glGetUniformLocation(progid, name);

            std::cout << std::boolalpha << prefix << sep
                      << std::right << std::setw(5)            << i                         << sep
                      << std::left  << std::setw(max_name_len) << name                      << sep
                      << std::right << std::setw(8)            << location                  << sep
                      << std::right << std::setw(11)           << block_indices[i]          << sep
                      << std::left  << std::setw(17)           << translateGLType(types[i]) << sep
                      << std::right << std::setw(4)            << sizes[i]                  << sep
                      << std::right << std::setw(6)            << offsets[i]                << sep
                      << std::right << std::setw(12)           << array_strides[i]          << sep
                      << std::right << std::setw(13)           << matrix_strides[i]         << sep
                      << std::left  << std::setw(10)           << (row_majors[i] != GL_FALSE) << sep
                      << std::endl;
        }


        delete [] name;
        delete [] indices;
        delete [] types;
        delete [] sizes;
        delete [] block_indices;
        delete [] offsets;
        delete [] array_strides;
        delete [] matrix_strides;
        delete [] row_majors;
    }

    void dumpOpenGLState() {
        GLint progid = -1, element_array_buffer = -1, vertex_array = -1;

        glGetIntegerv(GL_CURRENT_PROGRAM, &progid);
        std::cout << "OpenGL State:" << std::endl;
        std::cout << "  Current program: " << progid << std::endl;

        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vertex_array);
        std::cout << "    Vertex array object: " << vertex_array << std::endl;
        std::cout << std::endl;

        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &element_array_buffer);
        std::cout << "    Element array buffer: " << element_array_buffer << std::endl;
        std::cout << std::endl;

        dumpProgramAttributes(progid, "    ");
        std::cout << std::endl;

        dumpProgramUniforms(progid, "    ");
        std::cout << std::endl;

        // glGetProgramiv(progid, GL_ACTIVE_UNIFORM_BLOCKS, &num_things);
        // glGetProgramiv(progid, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &max_name_len);
        // name = new char[max_name_len];
        // std::cout << "    Uniform blocks: " << num_things << std::endl;
        // for (auto i = 0; i < num_things; ++i) {
        //     GLint binding = -1, bound_buffer = -1, num_uniforms = 0, data_size = 0;
        //     GLuint index = 0;
        //     glGetActiveUniformBlockiv(progid, i, GL_UNIFORM_BLOCK_BINDING, &binding);
        //     glGetActiveUniformBlockiv(progid, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &num_uniforms);
        //     glGetActiveUniformBlockiv(progid, i, GL_UNIFORM_BLOCK_DATA_SIZE, &data_size);
        //     glGetActiveUniformBlockName(progid, i, max_name_len, nullptr, name);
        //     glGetIntegeri_v(GL_UNIFORM_BUFFER_BINDING, binding, &bound_buffer);
        //     index = glGetUniformBlockIndex(progid, name);
        //     std::cout << "      " << i << ":"
        //               << " " << name << ":"
        //               << " index: " << index
        //               << " binding: " << binding
        //               << " num_uniforms: " << num_uniforms
        //               << " data_size: " << data_size
        //               << std::endl;

        //     GLint *int_uniform_indices = new GLint[num_uniforms];
        //     GLuint *uniform_indices = new GLuint[num_uniforms];
        //     GLint *sizes = new GLint[num_uniforms];
        //     GLint *itypes = new GLint[num_uniforms];
        //     GLint *offsets = new GLint[num_uniforms];
        //     GLint *array_strides = new GLint[num_uniforms];
        //     GLint *matrix_strides = new GLint[num_uniforms];
        //     GLint *row_majors = new GLint[num_uniforms];
        //     GLint max_name_len_2 = -1;

        //     glGetActiveUniformBlockiv(progid, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, int_uniform_indices);
        //     for (auto j = 0; j < num_uniforms; ++j) { uniform_indices[j] = (GLuint)int_uniform_indices[j]; }
            
        //     glGetActiveUniformsiv(progid, num_uniforms, uniform_indices, GL_UNIFORM_TYPE, itypes);
        //     glGetActiveUniformsiv(progid, num_uniforms, uniform_indices, GL_UNIFORM_SIZE, sizes);
        //     glGetActiveUniformsiv(progid, num_uniforms, uniform_indices, GL_UNIFORM_OFFSET, offsets);
        //     glGetActiveUniformsiv(progid, num_uniforms, uniform_indices, GL_UNIFORM_ARRAY_STRIDE, array_strides);
        //     glGetActiveUniformsiv(progid, num_uniforms, uniform_indices, GL_UNIFORM_MATRIX_STRIDE, matrix_strides);
        //     glGetActiveUniformsiv(progid, num_uniforms, uniform_indices, GL_UNIFORM_IS_ROW_MAJOR, row_majors);

        //     glGetProgramiv(progid, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len_2);
        //     char *name_2 = new char[max_name_len_2];

        //     for (auto j = 0; j < num_uniforms; ++j) {
        //         glGetActiveUniformName(progid, uniform_indices[j], max_name_len_2, nullptr, name_2);

        //         std::cout << "        " << j << ":"
        //                   << " " << name_2 << ":"
        //                   << " type: " << translateGLType(itypes[j])
        //                   << " size: " << sizes[j]
        //                   << " offset: " << offsets[j]
        //                   << " array stride: " << array_strides[j]
        //                   << " matrix stride: " << matrix_strides[j]
        //                   << " is row major: " << row_majors[j]
        //                   << std::endl;
        //     }

        //     delete [] int_uniform_indices;
        //     delete [] uniform_indices;
        //     delete [] sizes;
        //     delete [] itypes;
        //     delete [] offsets;
        //     delete [] array_strides;
        //     delete [] matrix_strides;
        //     delete [] row_majors;
        //     delete [] name_2;
        // }
        // delete [] name;

        // std::cout << std::endl;
    }
}
