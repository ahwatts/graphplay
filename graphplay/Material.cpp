// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "Material.h"

namespace graphplay {
    // Helper functions.

    // You need to do a delete [] on whatever's passed to shaders
    // after this method is called.
    void getAttachedShaders(GLuint program, int *num_shaders, GLuint **shaders) {
        glGetProgramiv(program, GL_ATTACHED_SHADERS, num_shaders);

        *shaders = new GLuint[*num_shaders];
        int returned_shaders = 0;
        glGetAttachedShaders(program, *num_shaders, &returned_shaders, *shaders);

        if (*num_shaders != returned_shaders) {
            std::cerr << "WFT? OpenGL says there's " << *num_shaders << " attached, but returned only " << returned_shaders << "?" << std::endl;
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
            glGetShaderInfoLog(shader, errlen - 1, NULL, err);
            std::cerr << "Could not compile shader!" << std::endl;
            std::cerr << "  error:" << std::endl << err << std::endl;
            std::cerr << "  source:" << std::endl << shader_src << std::endl;
            delete [] err;
            std::exit(1);
        }

        return shader;
    }

    // Class Material.
    Material::Material()
        : m_program(0) { }

    Material::~Material() {
        destroyProgram();
    }

    void Material::destroyProgram() {
        if (glIsProgram(m_program)) {
            int num_shaders = 0;
            GLuint *shaders = NULL;

            getAttachedShaders(m_program, &num_shaders, &shaders);

            for (int i = 0; i < num_shaders; ++i) {
                if (glIsShader(shaders[i])) {
                    glDetachShader(m_program, shaders[i]);
                    glDeleteShader(shaders[i]);
                }
            }

            delete [] shaders;

            glDeleteProgram(m_program);
            m_program = 0;
        }
    }

    GLuint Material::getProgram() const {
        return m_program;
    }

    GLuint Material::getVertexShader() const {
        int num_shaders = 0, shader_type = 0;
        GLuint *shaders = NULL, rv = 0;

        if (glIsProgram(m_program)) {
            getAttachedShaders(m_program, &num_shaders, &shaders);

            for (int i = 0; i < num_shaders; ++i) {
                if (glIsShader(shaders[i])) {
                    glGetShaderiv(shaders[i], GL_SHADER_TYPE, &shader_type);
                    if (shader_type == GL_VERTEX_SHADER) {
                        rv = shaders[i];
                        break;
                    }
                }
            }

            delete [] shaders;
        }

        return rv;
    }

    GLuint Material::getFragmentShader() const {
        int num_shaders = 0, shader_type = 0;
        GLuint *shaders = NULL, rv = 0;

        if (glIsProgram(m_program)) {
            getAttachedShaders(m_program, &num_shaders, &shaders);

            for (int i = 0; i < num_shaders; ++i) {
                if (glIsShader(shaders[i])) {
                    glGetShaderiv(shaders[i], GL_SHADER_TYPE, &shader_type);
                    if (shader_type == GL_FRAGMENT_SHADER) {
                        rv = shaders[i];
                        break;
                    }
                }
            }

            delete [] shaders;
        }

        return rv;
    }

    // Class GouraudMaterial.
    const char* GouraudMaterial::vertex_shader_src =
        "attribute vec3 aPosition;"
        "attribute vec4 aColor;"
        "uniform mat4 uModelView;"
        "uniform mat4 uProjection;"
        "varying vec4 vColor;"
        "void main(void) {"
        "    gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);"
        "    vColor = aColor;"
        "}";

    const char* GouraudMaterial::fragment_shader_src =
        "varying vec4 vColor;"
        "void main(void) {"
        "    gl_FragColor = vColor;"
        "}";

    GouraudMaterial::GouraudMaterial()
        : Material(),
          m_position_loc(0),
          m_color_loc(0),
          m_projection_loc(0),
          m_model_view_loc(0) { }

    GouraudMaterial::~GouraudMaterial() { }

    void GouraudMaterial::createProgram() {
        // implement me.
    }
};
