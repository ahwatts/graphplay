// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <iostream>

#include "Material.h"

namespace graphplay {
    void getAttachedShaders(GLuint program, int *num_shaders, GLuint **shaders) {
        glGetProgramiv(program, GL_ATTACHED_SHADERS, num_shaders);

        *shaders = new GLuint[*num_shaders];
        int returned_shaders = 0;
        glGetAttachedShaders(program, *num_shaders, &returned_shaders, *shaders);

        if (*num_shaders != returned_shaders) {
            std::cerr << "WFT? OpenGL says there's " << *num_shaders << " attached, but returned only " << returned_shaders << "?" << std::endl;
        }
    }

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
};
