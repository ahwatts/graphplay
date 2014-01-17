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
            delete [] err;

            std::exit(1);
        }

        return program;
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

    // Class LambertMaterial.
    const char* LambertMaterial::vertex_shader_src =
        "#version 430 core\n"

        "in vec3 aPosition;\n"
        "in vec3 aNormal;\n"
        "in vec4 aColor;\n"

        "uniform mat4x4 uModelView;\n"
        "uniform mat3x3 uModelViewInverse;\n"
        "uniform mat4x4 uProjection;\n"
        "uniform vec3 uLightPosition;\n"
        "uniform vec4 uLightColor;\n"

        "out vec4 vAmbientColor;\n"
        "out vec4 vDiffuseColor;\n"

        "void main(void) {\n"
        "    vec3 eye_vert_pos = (uModelView * vec4(aPosition, 1.0)).xyz;\n"
        "    vec3 eye_vert_norm = normalize(uModelViewInverse * aNormal);\n"
        "    vec3 eye_light_dir = normalize(uLightPosition - eye_vert_pos);\n"
        "    gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n"
        "    vAmbientColor = 0.05 * aColor;\n"
        "    vDiffuseColor = dot(eye_light_dir, eye_vert_norm) * uLightColor * aColor;\n"
        "}\n";

    const char* LambertMaterial::fragment_shader_src =
        "#version 430 core\n"

        "in vec4 vAmbientColor;\n"
        "in vec4 vDiffuseColor;\n"

        "out vec4 FragColor;\n"

        "void main(void) {\n"
        "    FragColor = clamp(vAmbientColor + vDiffuseColor, 0.0, 1.0);\n"
        "}\n";

    LambertMaterial::LambertMaterial()
        : Material(),
          m_position_loc(-1),
          m_normal_loc(-1),
          m_color_loc(-1),
          m_projection_loc(-1),
          m_model_view_loc(-1),
          m_model_view_inv_loc(-1),
          m_light_position_loc(-1),
          m_light_color_loc(-1) { }

    LambertMaterial::~LambertMaterial() { }

    void LambertMaterial::createProgram() {
        GLuint vertex_shader = createAndCompileShader(GL_VERTEX_SHADER, LambertMaterial::vertex_shader_src);
        GLuint fragment_shader = createAndCompileShader(GL_FRAGMENT_SHADER, LambertMaterial::fragment_shader_src);
        m_program = createProgramFromShaders(vertex_shader, fragment_shader);

        GLint pos_loc = glGetAttribLocation(m_program, "aPosition");
        GLint norm_loc = glGetAttribLocation(m_program, "aNormal");
        GLint color_loc = glGetAttribLocation(m_program, "aColor");

        if (pos_loc < 0 || color_loc < 0 || norm_loc < 0) {
            std::cerr << "aPosition, aColor, or aNormal could not be found in the shader program. "
                      << "Something is seriously wrong." << std::endl;
            std::exit(1);
        }

        m_position_loc = (GLuint)pos_loc;
        m_normal_loc = (GLuint)norm_loc;
        m_color_loc = (GLuint)color_loc;
        m_projection_loc = glGetUniformLocation(m_program, "uProjection");
        m_model_view_loc = glGetUniformLocation(m_program, "uModelView");
        m_model_view_inv_loc = glGetUniformLocation(m_program, "uModelViewInverse");
        m_light_position_loc = glGetUniformLocation(m_program, "uLightPosition");
        m_light_color_loc = glGetUniformLocation(m_program, "uLightColor");
    }

    const char *PhongMaterial::vertex_shader_src =
        "#version 430 core\n"

        "in vec3 aPosition;\n"
        "in vec3 aNormal;\n"
        "in vec4 aColor;\n"

        "uniform mat4x4 uModelView;\n"
        "uniform mat3x3 uModelViewInverse;\n"
        "uniform mat4x4 uProjection;\n"
        "uniform vec3 uLightPosition;\n"
        "uniform vec4 uLightColor;\n"

        "out vec4 vColor;\n"
        "out vec3 vEyeDir;\n"
        "out vec3 vNormal;\n"

        "void main(void) {\n"
        "    vec3 eye_vert_pos = (uModelView * vec4(aPosition, 1.0)).xyz;\n"

        "    vEyeDir = normalize(uLightPosition - eye_vert_pos);\n"
        "    vNormal = normalize(uModelViewInverse * aNormal);\n"
        "    vColor = aColor;\n"
        "    gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n"
        "}\n";

    const char *PhongMaterial::fragment_shader_src = 
        "#version 430 core\n"

        "in vec3 vEyeDir;\n"
        "in vec3 vNormal;\n"
        "in vec4 vColor;\n"

        "uniform vec4 uLightColor;\n"

        "out vec4 FragColor;\n"

        "void main(void) {\n"
        "    vec4 ambient_color = 0.05 * vColor;\n"
        "    vec4 diffuse_color = dot(vEyeDir, vNormal) * uLightColor * vColor;\n"
        "    FragColor = clamp(ambient_color + diffuse_color, 0.0, 1.0);\n"
        "}\n";

    PhongMaterial::PhongMaterial()
        : Material(),
          m_position_loc(-1),
          m_normal_loc(-1),
          m_color_loc(-1),
          m_projection_loc(-1),
          m_model_view_loc(-1),
          m_model_view_inv_loc(-1),
          m_light_position_loc(-1),
          m_light_color_loc(-1) { }

    PhongMaterial::~PhongMaterial() { }

    void PhongMaterial::createProgram() {
        GLuint vertex_shader = createAndCompileShader(GL_VERTEX_SHADER, PhongMaterial::vertex_shader_src);
        GLuint fragment_shader = createAndCompileShader(GL_FRAGMENT_SHADER, PhongMaterial::fragment_shader_src);
        m_program = createProgramFromShaders(vertex_shader, fragment_shader);

        GLint pos_loc = glGetAttribLocation(m_program, "aPosition");
        GLint normal_loc = glGetAttribLocation(m_program, "aNormal");

        if (pos_loc < 0 || normal_loc < 0) {
            std::cerr << "aPosition or aNormal could not be found in the shader program. "
                      << "Something is seriously wrong." << std::endl;
            std::exit(1);
        }

        m_position_loc = (GLuint)pos_loc;
        m_normal_loc = (GLuint)normal_loc;
        m_projection_loc = glGetUniformLocation(m_program, "uProjection");
        m_model_view_loc = glGetUniformLocation(m_program, "uModelView");
        m_model_view_inv_loc = glGetUniformLocation(m_program, "uModelViewInverse");
        m_light_position_loc = glGetUniformLocation(m_program, "uLightPosition");
        m_light_color_loc = glGetUniformLocation(m_program, "uLightColor");
    }
};
