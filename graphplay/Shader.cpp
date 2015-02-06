// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "opengl.h"

#include "Shader.h"

namespace graphplay {
    // Helper function declarations.
    GLuint createAndCompileShader(GLenum shader_type, const char* shader_src);
    GLuint createProgramFromShaders(GLuint vertex_shader, GLuint fragment_shader);
    void getAttachedShaders(GLuint program, std::vector<GLuint> &shaders);
    void getAttributeInfo(GLuint program, Shader::index_map &attributes);
    void getUniformInfo(GLuint program, Shader::index_map &uniforms);

    Shader::Shader(const char *vertex_shader_source, const char *fragment_shader_source)
        : m_program(0),
          m_attributes(),
          m_uniforms(),
          m_uniform_blocks()
    {
        GLuint vshader = createAndCompileShader(GL_VERTEX_SHADER, vertex_shader_source);
        GLuint fshader = createAndCompileShader(GL_FRAGMENT_SHADER, fragment_shader_source);
        m_program = createProgramFromShaders(vshader, fshader);
        getAttributeInfo(m_program, m_attributes);
        getUniformInfo(m_program, m_uniforms);
    }

    Shader::~Shader() {
        if (glIsProgram(m_program)) {
            std::vector<GLuint> shaders;

            getAttachedShaders(m_program, shaders);

            for (auto s : shaders) {
                if (glIsShader(s)) {
                    glDetachShader(m_program, s);
                    glDeleteShader(s);
                }
            }

            glDeleteProgram(m_program);
        }

        m_program = 0;
        m_attributes.clear();
        m_uniforms.clear();
        m_uniform_blocks.clear();
    }

    void Shader::dump() const {
        std::cout << "<Shader m_program = " << m_program;

        std::cout << " attributes = { ";
        for (const auto a : m_attributes) {
            std::cout << a.first << ": " << a.second;
            if (a != *m_attributes.crbegin()) {
                std::cout << ", ";
            }
        }
        std::cout << " }";

        std::cout << " uniforms = { ";
        for (auto a : m_uniforms) {
            std::cout << a.first << ": " << a.second;
            if (a != *m_uniforms.crbegin()) {
                std::cout << ", ";
            }
        }
        std::cout << " }";

        std::cout << ">" << std::endl;
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

    void getAttributeInfo(GLuint program, Shader::index_map &attributes) {
        GLint num_attrs, max_name_len;
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &num_attrs);
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_name_len);

        char *name = new char[max_name_len];

        for (unsigned int i = 0; i < (unsigned int)num_attrs; ++i) {
            GLsizei name_len = 0, size = 0;
            GLenum type = 0;
            glGetActiveAttrib(program, i, max_name_len, &name_len, &size, &type, name);
            attributes[name] = i;
        }

        delete [] name;
    }

    void getUniformInfo(GLuint program, Shader::index_map &uniforms) {
        GLint num_unifs, max_name_len;
        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &num_unifs);
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);

        char *name = new char[max_name_len];

        for (unsigned int i = 0; i < (unsigned int)num_unifs; ++i) {
            GLsizei name_len = 0, size = 0;
            GLenum type = 0;
            glGetActiveUniform(program, i, max_name_len, &name_len, &size, &type, name);
            uniforms[name] = i;
        }

        delete[] name;
    }
}
