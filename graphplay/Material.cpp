// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#include "Material.h"

namespace graphplay {
    // Helper functions.

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

        delete [] shader_return;
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
            delete [] err;
            std::exit(1);
        }

        return shader;
    }

    // Class Material.
    Material::Material() : m_program(0) { }

    Material::~Material() {
        destroyProgram();
    }

    void Material::destroyProgram() {
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
            m_program = 0;
        }
    }

    GLuint Material::getProgram() const {
        return m_program;
    }

    GLuint Material::getVertexShader() const {
        if (glIsProgram(m_program)) {
            std::vector<GLuint> shaders;
            getAttachedShaders(m_program, shaders);

            for (auto s : shaders) {
                if (glIsShader(s)) {
                    int shader_type = 0;
                    glGetShaderiv(s, GL_SHADER_TYPE, &shader_type);

                    if (shader_type == GL_VERTEX_SHADER) {
                        return s;
                    }
                }
            }
        }

        return 0;
    }

    GLuint Material::getFragmentShader() const {
        if (glIsProgram(m_program)) {
            std::vector<GLuint> shaders;
            getAttachedShaders(m_program, shaders);

            for (auto s : shaders) {
                if (glIsShader(s)) {
                    int shader_type = 0;
                    glGetShaderiv(s, GL_SHADER_TYPE, &shader_type);
                    if (shader_type == GL_FRAGMENT_SHADER) {
                        return s;
                    }
                }
            }
        }

        return 0;
    }

    // Class GouraudMaterial.
    const char* GouraudMaterial::vertex_shader_src =
        "attribute vec3 aPosition;\n"
        "attribute vec4 aColor;\n"
        "uniform mat4 uModelView;\n"
        "uniform mat4 uProjection;\n"
        "varying vec4 vColor;\n"
        "void main(void) {\n"
        "    gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n"
        "    vColor = aColor;\n"
        "}\n";

    const char* GouraudMaterial::fragment_shader_src =
        "varying vec4 vColor;\n"
        "void main(void) {\n"
        "    gl_FragColor = vColor;\n"
        "}\n";

    GouraudMaterial::GouraudMaterial()
        : Material(),
          m_position_loc(-1),
          m_color_loc(-1),
          m_projection_loc(-1),
          m_model_view_loc(-1) { }

    GouraudMaterial::~GouraudMaterial() { }

    void GouraudMaterial::createProgram() {
        GLuint vertex_shader = createAndCompileShader(GL_VERTEX_SHADER, GouraudMaterial::vertex_shader_src);
        GLuint fragment_shader = createAndCompileShader(GL_FRAGMENT_SHADER, GouraudMaterial::fragment_shader_src);

        m_program = glCreateProgram();
        glAttachShader(m_program, vertex_shader);
        glAttachShader(m_program, fragment_shader);
        glLinkProgram(m_program);

        GLint status;
        glGetProgramiv(m_program, GL_LINK_STATUS, &status);
        if (!status) {
            GLint errlen;
            glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &errlen);

            char *err = new char[errlen];
            glGetProgramInfoLog(m_program, errlen, NULL, err);
            std::cerr << "Could not link shader program: " << err << std::endl;
            delete [] err;

            std::exit(1);
        }

        GLint pos_loc = glGetAttribLocation(m_program, "aPosition");
        GLint color_loc = glGetAttribLocation(m_program, "aColor");

        if (pos_loc < 0 || color_loc < 0) {
            std::cerr << "aPosition or aColor could not be found in the shader program. "
                      << "Something is seriously wrong." << std::endl;
            std::exit(1);
        }

        m_position_loc = (GLuint)pos_loc;
        m_color_loc = (GLuint)color_loc;
        m_projection_loc = glGetUniformLocation(m_program, "uProjection");
        m_model_view_loc = glGetUniformLocation(m_program, "uModelView");
    }
};
