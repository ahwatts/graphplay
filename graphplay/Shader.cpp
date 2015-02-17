// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <iostream>

#include "Shader.h"

namespace graphplay {
    // Shader class.
    Shader::Shader(GLenum type, const char *source) : m_shader{0} {
        m_shader = createAndCompileShader(type, source);
    }

    Shader::~Shader() {
        if (glIsShader(m_shader)) {
            glDeleteShader(m_shader);
        }
    }

    // Program class.
    Program::Program(Shader::sptr_type vertex_shader, Shader::sptr_type fragment_shader)
        : m_program{0},
          m_vertex_shader{vertex_shader},
          m_fragment_shader{fragment_shader},
          m_attributes(),
          m_uniforms(),
          m_uniform_blocks()
    {
        m_program = createProgramFromShaders(
            m_vertex_shader->getShaderId(),
            m_fragment_shader->getShaderId());
        getAttributeInfo(m_program, m_attributes);
        getUniformInfo(m_program, m_uniforms);
        getUniformBlockInfo(m_program, m_uniform_blocks);
    }

    Program::Program(const Program &other)
        : m_program{0},
          m_vertex_shader{other.m_vertex_shader},
          m_fragment_shader{other.m_fragment_shader},
          m_attributes(),
          m_uniforms(),
          m_uniform_blocks()
    {
        m_program = createProgramFromShaders(
            m_vertex_shader->getShaderId(),
            m_fragment_shader->getShaderId());
        getAttributeInfo(m_program, m_attributes);
        getUniformInfo(m_program, m_uniforms);
        getUniformBlockInfo(m_program, m_uniform_blocks);
    }

    Program::Program(Program &&other)
        : m_program{other.m_program},
          m_vertex_shader{other.m_vertex_shader},
          m_fragment_shader{other.m_fragment_shader},
          m_attributes(),
          m_uniforms(),
          m_uniform_blocks()
    {
        other.m_program = 0;
        std::swap(m_attributes, other.m_attributes);
        std::swap(m_uniforms, other.m_uniforms);
        std::swap(m_uniform_blocks, other.m_uniform_blocks);
    }

    Program::~Program() {
        if (glIsProgram(m_program)) {
            std::vector<GLuint> shaders;
            getAttachedShaders(m_program, shaders);

            for (auto s : shaders) {
                if (glIsShader(s)) {
                    // The shader *should* get deleted when its 
                    // shared pointers go out of scope...
                    glDetachShader(m_program, s);
                }
            }

            glDeleteProgram(m_program);
            m_program = 0;
        }
    }

    Program& Program::operator=(const Program &other) {
        Program tmp(other);
        std::swap(*this, tmp);
        return *this;
    }

    Program& Program::operator=(Program &&other) {
        std::swap(m_program, other.m_program);
        std::swap(m_vertex_shader, other.m_vertex_shader);
        std::swap(m_fragment_shader, other.m_fragment_shader);
        std::swap(m_attributes, other.m_attributes);
        std::swap(m_uniforms, other.m_uniforms);
        std::swap(m_uniform_blocks, other.m_uniform_blocks);
        return *this;
    }

    // Actual shader code.
    const char *Shader::unlit_vertex_shader_source = R"glsl(
        #version 410 core

        in vec3 position;
        in vec4 color;

        uniform mat4x4 model;
        uniform mat3x3 model_inv_trans_3;
        uniform view_and_projection {
            mat4x4 view;
            mat4x4 view_inv;
            mat4x4 projection;
        };

        out vec4 v_color;

        void main(void) {
            gl_Position = projection * view * model * vec4(position, 1.0);
            v_color = color;
        }
    )glsl";

    const char *Shader::unlit_fragment_shader_source = R"glsl(
        #version 410 core

        in vec4 v_color;

        out vec4 FragColor;

        void main(void) {
            FragColor = v_color;
        }
    )glsl";
}
