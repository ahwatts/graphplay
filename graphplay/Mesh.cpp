// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "Mesh.h"
#include "Material.h"
#include "graphplay.h"

#include <iostream>

namespace graphplay {
    Mesh::Mesh() : m_model_transform(), m_geometry(), m_material() { }
    Mesh::Mesh(sp_Geometry geo, sp_Material mat) : m_model_transform(), m_geometry(geo), m_material(mat) { }

    void Mesh::setGeometry(sp_Geometry geo) { m_geometry = geo; }
    void Mesh::setMaterial(sp_Material mat) { m_material = mat; }

    void Mesh::setTransform(const glm::mat4x4 &new_transform) {
        m_model_transform = new_transform;
    }

    void Mesh::render(const glm::mat4x4 &projection, const glm::mat4x4 &model_view) const {
        glm::mat4x4 full_mv = model_view * m_model_transform;
        m_geometry->render(projection, full_mv, *m_material);
    }

    // Class DebugMesh.
    const char *DebugMesh::vertex_shader_src = GLSL(
        in vec3 aPosition;
        in vec3 aNormal;
        in vec4 aColor;

        uniform mat4x4 uModelView;
        uniform mat3x3 uModelViewInverse;
        uniform mat4x4 uProjection;
        uniform vec3 uLightPosition;
        uniform vec4 uLightColor;
        uniform uint uSpecularExponent;

        out vec4 vAmbientColor;
        out vec4 vDiffuseColor;
        out vec4 vSpecularColor;

        // For the transform feedback.
        out vec3 eye_light_dir;
        out vec3 eye_eye_dir;
        out vec3 eye_reflected_dir;

        void main(void) {
            vec3 eye_vert_pos = vec3(uModelView * vec4(aPosition, 1.0));
            vec3 eye_vert_norm = normalize(uModelViewInverse * aNormal);
            eye_light_dir = normalize(uLightPosition - eye_vert_pos);
            eye_eye_dir = normalize(eye_vert_pos);
            eye_reflected_dir = normalize(2 * dot(eye_light_dir, eye_vert_norm) * eye_vert_norm - eye_light_dir);

            gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);
            vAmbientColor = 0.05 * aColor;
            vDiffuseColor = dot(eye_light_dir, eye_vert_norm) * uLightColor * aColor;
            vSpecularColor = pow(dot(eye_reflected_dir, eye_eye_dir), uSpecularExponent) * uLightColor * aColor;
        }
    );

    const char *DebugMesh::fragment_shader_src = GLSL(
        in vec4 vAmbientColor;
        in vec4 vDiffuseColor;
        in vec4 vSpecularColor;

        out vec4 FragColor;

        void main(void) {
            FragColor = clamp(vAmbientColor + vDiffuseColor + vSpecularColor, 0.0, 1.0);
        }
    );

    DebugMesh::DebugMesh(sp_Geometry geo) : Mesh() {
        GLuint vertex_shader = createAndCompileShader(GL_VERTEX_SHADER, DebugMesh::vertex_shader_src);
        GLuint fragment_shader = createAndCompileShader(GL_FRAGMENT_SHADER, DebugMesh::fragment_shader_src);
        const char *varying_names[] = {
            "eye_light_dir", "eye_eye_dir", "eye_reflected_dir"
        };

        GLuint program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glTransformFeedbackVaryings(program, 3, varying_names, GL_INTERLEAVED_ATTRIBS);
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

        m_position_loc = (GLuint)glGetAttribLocation(m_program, "aPosition");
        m_normal_loc = (GLuint)glGetAttribLocation(m_program, "aNormal");
        m_color_loc = (GLuint)glGetAttribLocation(m_program, "aColor");

        m_projection_loc = glGetUniformLocation(m_program, "uProjection");
        m_model_view_loc = glGetUniformLocation(m_program, "uModelView");
        m_model_view_inv_loc = glGetUniformLocation(m_program, "uModelViewInverse");
        m_light_position_loc = glGetUniformLocation(m_program, "uLightPosition");
        m_light_color_loc = glGetUniformLocation(m_program, "uLightColor");
        m_specular_exponent_loc = glGetUniformLocation(m_program, "uSpecularExponent");
    }

    void DebugMesh::render(const glm::mat4x4 &projection, const glm::mat4x4 &model_view) const {
        glm::mat4x4 full_mv = model_view * m_model_transform;
        glm::mat3x3 mv_inverse = glm::inverseTranspose(glm::mat3x3(full_mv));
    }
};
