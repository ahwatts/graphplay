// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "opengl.h"

#include "Shader.h"

namespace graphplay {
    // Shader class
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
        getUniformBlockInfo(m_program, m_uniform_blocks);
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

        std::cout << " uniform blocks = { ";
        for (auto a : m_uniform_blocks) {
            std::cout << a.first << ": " << a.second;
            if (a != *m_uniform_blocks.crbegin()) {
                std::cout << ", ";
            }
        }
        std::cout << " }";

        std::cout << ">" << std::endl;
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
