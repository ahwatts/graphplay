// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "graphplay.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#include "Material.h"
#include "opengl.h"

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

    // Class GouraudMaterial.
    const char *GouraudMaterial::vertex_shader_src = R"glsl(
        #version 410 core

        in vec3 aPosition;
        in vec4 aColor;

        uniform mat4x4 uModel;
        uniform mat4x4 uView;
        uniform mat4x4 uProjection;

        out vec4 vColor;

        void main(void) {
            gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
            vColor = aColor;
        }
    )glsl";

    const char *GouraudMaterial::fragment_shader_src = R"glsl(
        #version 410 core

        in vec4 vColor;

        out vec4 FragColor;

        void main(void) {
            FragColor = vColor;
        }
    )glsl";

    GouraudMaterial::GouraudMaterial()
        : Material(),
          m_position_loc(-1),
          m_color_loc(-1),
          m_model_loc(-1),
          m_view_loc(-1),
          m_projection_loc(-1) {}

    GouraudMaterial::~GouraudMaterial() { }

    void GouraudMaterial::createProgram() {
        GLuint vertex_shader = createAndCompileShader(GL_VERTEX_SHADER, GouraudMaterial::vertex_shader_src);
        GLuint fragment_shader = createAndCompileShader(GL_FRAGMENT_SHADER, GouraudMaterial::fragment_shader_src);
        m_program = createProgramFromShaders(vertex_shader, fragment_shader);

        m_position_loc = (GLuint)glGetAttribLocation(m_program, "aPosition");
        m_color_loc = (GLuint)glGetAttribLocation(m_program, "aColor");

        m_model_loc = glGetUniformLocation(m_program, "uModel");
        m_view_loc = glGetUniformLocation(m_program, "uView");
        m_projection_loc = glGetUniformLocation(m_program, "uProjection");
    }

    // Class LambertMaterial.
    const char* LambertMaterial::vertex_shader_src = R"glsl(
        #version 410 core

        in vec3 aPosition;
        in vec3 aNormal;
        in vec4 aColor;

        uniform mat4x4 uModel;
        uniform mat3x3 uModelInverseTranspose3;
        uniform mat4x4 uView;
        uniform mat4x4 uViewInverse;
        uniform mat4x4 uProjection;

        uniform vec3 uLightPosition;
        uniform vec4 uLightColor;
        uniform uint uSpecularExponent;

        out vec4 vAmbientColor;
        out vec4 vDiffuseColor;
        out vec4 vSpecularColor;

        void main(void) {
            vec4 wld_vert_position4 = uModel * vec4(aPosition, 1.0);
            vec3 wld_vert_position = wld_vert_position4.xyz / wld_vert_position4.w;

            vec4 wld_eye_position4 = uViewInverse * vec4(0.0, 0.0, 0.0, 1.0);
            vec3 wld_eye_position = wld_eye_position4.xyz / wld_eye_position4.w;

            vec3 wld_vert_normal = normalize(uModelInverseTranspose3 * aNormal);

            vec3 wld_vert_light_dir = normalize(uLightPosition - wld_vert_position);
            vec3 wld_vert_light_reflect_dir = normalize(reflect(-1 * wld_vert_light_dir, wld_vert_normal));
            vec3 wld_vert_eye_dir = normalize(wld_eye_position - wld_vert_position);

            gl_Position = uProjection * uView * wld_vert_position4;

            vAmbientColor = vec4(0.1 * aColor.rgb, aColor.a);

            float diffuse_coeff = 0.7 * max(0.0, dot(wld_vert_normal, wld_vert_light_dir));
            vDiffuseColor = vec4(diffuse_coeff * uLightColor.rgb * aColor.rgb, aColor.a);

            if (dot(wld_vert_normal, wld_vert_light_dir) < 0.0) {
                vSpecularColor = vec4(0.0, 0.0, 0.0, 1.0);
            } else {
                float spec_coeff = 0.7 * pow(max(0.0, dot(wld_vert_light_reflect_dir, wld_vert_eye_dir)), uSpecularExponent);
                vSpecularColor = vec4(spec_coeff * uLightColor.rgb * aColor.rgb, aColor.a);
            }
        }
    )glsl";

    const char* LambertMaterial::fragment_shader_src = R"glsl(
        #version 410 core

        in vec4 vAmbientColor;
        in vec4 vDiffuseColor;
        in vec4 vSpecularColor;

        out vec4 FragColor;

        void main(void) {
            vec3 ambient_rgb = vec3(vAmbientColor);
            vec3 diffuse_rgb = vec3(vDiffuseColor);
            vec3 specular_rgb = vec3(vSpecularColor);
            FragColor = vec4(clamp(ambient_rgb + diffuse_rgb + specular_rgb, 0.0, 1.0), 1.0);
        }
    )glsl";

    LambertMaterial::LambertMaterial()
        : Material(),
          m_position_loc(-1),
          m_normal_loc(-1),
          m_color_loc(-1),
          m_model_loc(-1),
          m_model_inv_trans_3_loc(-1),
          m_view_loc(-1),
          m_view_inv_loc(-1),
          m_projection_loc(-1),
          m_light_position_loc(-1),
          m_light_color_loc(-1),
          m_specular_exponent_loc(-1) { }

    LambertMaterial::~LambertMaterial() { }

    void LambertMaterial::createProgram() {
        GLuint vertex_shader = createAndCompileShader(GL_VERTEX_SHADER, LambertMaterial::vertex_shader_src);
        GLuint fragment_shader = createAndCompileShader(GL_FRAGMENT_SHADER, LambertMaterial::fragment_shader_src);
        m_program = createProgramFromShaders(vertex_shader, fragment_shader);

        m_position_loc = (GLuint)glGetAttribLocation(m_program, "aPosition");
        m_normal_loc = (GLuint)glGetAttribLocation(m_program, "aNormal");
        m_color_loc = (GLuint)glGetAttribLocation(m_program, "aColor");

        m_model_loc = glGetUniformLocation(m_program, "uModel");
        m_model_inv_trans_3_loc = glGetUniformLocation(m_program, "uModelInverseTranspose3");
        m_view_loc = glGetUniformLocation(m_program, "uView");
        m_view_inv_loc = glGetUniformLocation(m_program, "uViewInverse");
        m_projection_loc = glGetUniformLocation(m_program, "uProjection");

        m_light_position_loc = glGetUniformLocation(m_program, "uLightPosition");
        m_light_color_loc = glGetUniformLocation(m_program, "uLightColor");
        m_specular_exponent_loc = glGetUniformLocation(m_program, "uSpecularExponent");
    }

    const char *PhongMaterial::vertex_shader_src = R"glsl(
        #version 410 core

        in vec3 aPosition;
        in vec3 aNormal;
        in vec4 aColor;

        uniform mat4x4 uModel;
        uniform mat3x3 uModelInverseTranspose3;
        uniform mat4x4 uView;
        uniform mat4x4 uViewInverse;
        uniform mat4x4 uProjection;

        uniform vec3 uLightPosition;
        uniform vec4 uLightColor;
        uniform uint uSpecularExponent;

        out vec4 vColor;
        out vec3 vEyeDir;
        out vec3 vNormal;

        void main(void) {
            vec4 position = vec4(aPosition, 1.0);
            vec3 eye_vert_pos = (uModelView * position).xyz;

            vEyeDir = normalize(uLightPosition - eye_vert_pos);
            vNormal = normalize(uModelViewInverse * aNormal);
            vColor = aColor;
            gl_Position = uProjection * uModelView * position;
        }
    )glsl";

    const char *PhongMaterial::fragment_shader_src = R"glsl(
        #version 410 core

        in vec4 vColor;
        in vec3 vEyeDir;
        in vec3 vNormal;

        uniform vec4 uLightColor;

        out vec4 FragColor;

        void main(void) {
            vec4 ambient_color = 0.05 * vColor;
            vec4 diffuse_color = dot(vEyeDir, vNormal) * uLightColor * vColor;
            FragColor = clamp(ambient_color + diffuse_color, 0.0, 1.0);
        }
    )glsl";

    PhongMaterial::PhongMaterial()
        : Material(),
          m_position_loc(-1),
          m_normal_loc(-1),
          m_color_loc(-1),
          m_model_loc(-1),
          m_model_inv_trans_3_loc(-1),
          m_view_loc(-1),
          m_view_inv_loc(-1),
          m_projection_loc(-1),
          m_light_position_loc(-1),
          m_light_color_loc(-1),
          m_specular_exponent_loc(-1) { }

    PhongMaterial::~PhongMaterial() { }

    void PhongMaterial::createProgram() {
        GLuint vertex_shader = createAndCompileShader(GL_VERTEX_SHADER, PhongMaterial::vertex_shader_src);
        GLuint fragment_shader = createAndCompileShader(GL_FRAGMENT_SHADER, PhongMaterial::fragment_shader_src);
        m_program = createProgramFromShaders(vertex_shader, fragment_shader);

        m_position_loc = (GLuint)glGetAttribLocation(m_program, "aPosition");
        m_normal_loc = (GLuint)glGetAttribLocation(m_program, "aNormal");
        m_color_loc = (GLuint)glGetAttribLocation(m_program, "aColor");

        m_model_loc = glGetUniformLocation(m_program, "uModel");
        m_model_inv_trans_3_loc = glGetUniformLocation(m_program, "uModelInverseTranspose3");
        m_view_loc = glGetUniformLocation(m_program, "uView");
        m_view_inv_loc = glGetUniformLocation(m_program, "uViewInverse");
        m_projection_loc = glGetUniformLocation(m_program, "uProjection");

        m_light_position_loc = glGetUniformLocation(m_program, "uLightPosition");
        m_light_color_loc = glGetUniformLocation(m_program, "uLightColor");
        m_specular_exponent_loc = glGetUniformLocation(m_program, "uSpecularExponent");
    }
};
