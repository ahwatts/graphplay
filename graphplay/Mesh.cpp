// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "Mesh.h"
#include "Material.h"
#include "graphplay.h"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace graphplay {
    void copy_mat4x4_to_array(float *arr, const glm::mat4x4 &mat) {
        const float *ptr = glm::value_ptr(mat);
        unsigned int i = 0;
        for (i = 0; i < 16; ++i) {
            arr[i] = ptr[i];
        }
    }

    Mesh::Mesh() : m_geometry(), m_material()
    {
        copy_mat4x4_to_array(m_model_transform, glm::mat4x4());
    }
    
    Mesh::Mesh(sp_Geometry geo, sp_Material mat) : m_geometry(geo), m_material(mat)
    {
        copy_mat4x4_to_array(m_model_transform, glm::mat4x4());
    }

    void Mesh::setGeometry(sp_Geometry geo) { m_geometry = geo; }
    void Mesh::setMaterial(sp_Material mat) { m_material = mat; }

    void Mesh::setTransform(const glm::mat4x4 &new_transform) {
        copy_mat4x4_to_array(m_model_transform, new_transform);
    }

    void Mesh::render(const glm::mat4x4 &projection, const glm::mat4x4 &model_view) const {
        glm::mat4x4 transform_mat = glm::make_mat4x4(m_model_transform);
        glm::mat4x4 full_mv = model_view * transform_mat;
        m_geometry->render(projection, full_mv, *m_material);
    }

    // Class DebugMesh.
    const char *DebugMesh::vertex_shader_1_src = GLSL("130",
        in vec3 aPosition;
        in vec3 aNormal;
        in vec4 aColor;

        uniform mat4x4 uModelView;
        uniform mat3x3 uModelViewInverse;
        uniform mat4x4 uProjection;
        uniform vec3 uLightPosition;
        uniform vec4 uLightColor;
        uniform uint uSpecularExponent;

        // For the fragment shader.
        out vec4 vAmbientColor;
        out vec4 vDiffuseColor;
        out vec4 vSpecularColor;

        // For the transform feedback.
        out vec3 light_src;
        out vec3 light_dst;
        out vec3 eye_src;
        out vec3 eye_dst;
        out vec3 reflected_src;
        out vec3 reflected_dst;

        void main(void) {
            vec3 eye_vert_pos = vec3(uModelView * vec4(aPosition, 1.0));
            vec3 eye_light_pos = uLightPosition;
            vec3 eye_vert_norm = normalize(uModelViewInverse * aNormal);
            vec3 eye_light_dir = normalize(eye_light_pos - eye_vert_pos);
            vec3 eye_eye_dir = normalize(eye_vert_pos);
            vec3 eye_reflected_dir = normalize(2 * dot(eye_light_dir, eye_vert_norm) * eye_vert_norm - eye_light_dir);

            light_src = eye_vert_pos;
            light_dst = eye_vert_pos + 0.5*eye_light_dir;

            eye_src = eye_vert_pos;
            eye_dst = eye_vert_pos + 0.5*eye_eye_dir;

            reflected_src = eye_vert_pos;
            reflected_dst = eye_vert_pos + 0.5*eye_reflected_dir;

            gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);
            vAmbientColor = 0.05 * aColor;
            vDiffuseColor = dot(eye_light_dir, eye_vert_norm) * uLightColor * aColor;
            vSpecularColor = pow(dot(eye_reflected_dir, eye_eye_dir), uSpecularExponent) * uLightColor * aColor;
        }
    );

    const char *DebugMesh::fragment_shader_1_src = GLSL("130",
        in vec4 vAmbientColor;
        in vec4 vDiffuseColor;
        in vec4 vSpecularColor;

        out vec4 FragColor;

        void main(void) {
            FragColor = clamp(vAmbientColor + vDiffuseColor + vSpecularColor, 0.0, 1.0);
        }
    );

    const char *DebugMesh::vertex_shader_2_src = GLSL("130",
        in vec3 aPosition;

        uniform mat4x4 uProjection;

        void main(void) {
            gl_Position = uProjection * vec4(aPosition, 1.0);
        }
    );

    const char *DebugMesh::fragment_shader_2_src = GLSL("130",
        out vec4 FragColor;

        void main(void) {
            FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        }
    );

    DebugMesh::DebugMesh(sp_Geometry geo) : Mesh() {
        m_geometry = geo;

        // Build the first-pass shader program.
        GLuint vertex_shader = createAndCompileShader(GL_VERTEX_SHADER, DebugMesh::vertex_shader_1_src);
        GLuint fragment_shader = createAndCompileShader(GL_FRAGMENT_SHADER, DebugMesh::fragment_shader_1_src);
        m_program_1 = createProgramFromShaders(vertex_shader, fragment_shader);

        // Re-link it with the varying names.
        const char *varying_names[] = { "light_src", "eye_src", "reflected_src", "light_dst", "eye_dst", "reflected_dst" };
        glTransformFeedbackVaryings(m_program_1, 6, varying_names, GL_INTERLEAVED_ATTRIBS);
        glLinkProgram(m_program_1);
        checkProgramLinkStatus(m_program_1);

        m_position_loc = (GLuint)glGetAttribLocation(m_program_1, "aPosition");
        m_normal_loc = (GLuint)glGetAttribLocation(m_program_1, "aNormal");
        m_color_loc = (GLuint)glGetAttribLocation(m_program_1, "aColor");

        m_projection_loc = glGetUniformLocation(m_program_1, "uProjection");
        m_model_view_loc = glGetUniformLocation(m_program_1, "uModelView");
        m_model_view_inv_loc = glGetUniformLocation(m_program_1, "uModelViewInverse");
        m_light_position_loc = glGetUniformLocation(m_program_1, "uLightPosition");
        m_light_color_loc = glGetUniformLocation(m_program_1, "uLightColor");
        m_specular_exponent_loc = glGetUniformLocation(m_program_1, "uSpecularExponent");

        unsigned int num_geo_verts = m_geometry->getNumVertices();

        // Create the buffer to receive the transform feedback. It will contain six vec3's per vertex.
        glGenBuffers(1, &m_feedback_buffer);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m_feedback_buffer);
        glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, 6*3*num_geo_verts*sizeof(float), NULL, GL_DYNAMIC_COPY);

        // Build the second-pass shader program.
        vertex_shader = createAndCompileShader(GL_VERTEX_SHADER, DebugMesh::vertex_shader_2_src);
        fragment_shader = createAndCompileShader(GL_FRAGMENT_SHADER, DebugMesh::fragment_shader_2_src);
        m_program_2 = createProgramFromShaders(vertex_shader, fragment_shader);

        m_position_loc_2 = (GLuint)glGetAttribLocation(m_program_2, "aPosition");
        m_projection_loc_2 = glGetUniformLocation(m_program_2, "uProjection");
    }

    void DebugMesh::render(const glm::mat4x4 &projection, const glm::mat4x4 &model_view) const {
        glm::mat4x4 model_transform = glm::make_mat4x4(m_model_transform);
        glm::mat4x4 full_mv = model_view * model_transform;
        glm::mat3x3 mv_inverse = glm::inverseTranspose(glm::mat3x3(full_mv));
        glm::vec3 light_pos(2, 2, 10);
        glm::vec4 light_color(0.25, 1.0, 0.5, 1.0);
        unsigned int specular_exponent = 2;

        GLsizeiptr vertex_size = m_geometry->getStride() * sizeof(float);

        // Use our special shader program.
        glUseProgram(m_program_1);

        // Use our geometry's data buffer.
        glBindBuffer(GL_ARRAY_BUFFER, m_geometry->getArrayBuffer());

        // Use the data buffer to the various shader inputs.
        glEnableVertexAttribArray(m_position_loc);
        glVertexAttribPointer(m_position_loc, 3, GL_FLOAT, GL_FALSE, vertex_size,
            BUFFER_OFFSET_BYTES(m_geometry->getPositionOffset()*sizeof(float)));

        glEnableVertexAttribArray(m_normal_loc);
        glVertexAttribPointer(m_normal_loc, 3, GL_FLOAT, GL_FALSE, vertex_size,
            BUFFER_OFFSET_BYTES(m_geometry->getNormalOffset()*sizeof(float)));

        glEnableVertexAttribArray(m_color_loc);
        glVertexAttribPointer(m_color_loc, 4, GL_FLOAT, GL_FALSE, vertex_size,
            BUFFER_OFFSET_BYTES(m_geometry->getColorOffset()*sizeof(float)));

        // Set the inputs for the uniforms.
        glUniformMatrix4fv(m_projection_loc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(m_model_view_loc, 1, GL_FALSE, glm::value_ptr(full_mv));
        glUniformMatrix3fv(m_model_view_inv_loc, 1, GL_FALSE, glm::value_ptr(mv_inverse));
        glUniform3fv(m_light_position_loc, 1, glm::value_ptr(light_pos));
        glUniform4fv(m_light_color_loc, 1, glm::value_ptr(light_color));
        glUniform1ui(m_specular_exponent_loc, specular_exponent);

        // Set the element array buffer.
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_geometry->getElementArrayBuffer());

        // Set the transform feedback buffer.
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_feedback_buffer);

        // Render with transform feedback.
        glBeginTransformFeedback(m_geometry->getDrawType());
        glDrawElements(m_geometry->getDrawType(), m_geometry->getNumVertices(), GL_UNSIGNED_INT, 0);
        glEndTransformFeedback();

        // Render the feedback.
        glUseProgram(m_program_2);

        glBindBuffer(GL_ARRAY_BUFFER, m_feedback_buffer);
        glEnableVertexAttribArray(m_position_loc_2);
        glVertexAttribPointer(m_position_loc_2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), BUFFER_OFFSET_BYTES(0*sizeof(float)));

        glUniformMatrix4fv(m_projection_loc_2, 1, GL_FALSE, glm::value_ptr(projection));

        glDrawArrays(GL_LINES, 0, 2*m_geometry->getNumVertices());
    }

    void DebugMesh::printTransformFeedback() const {
        float *feedback = NULL;
        
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m_feedback_buffer);
        feedback = (float*)glMapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, GL_READ_ONLY);

        if (!feedback) {
            std::cout << "Could not map feedback buffer!" << std::endl;
            return;
        }
        
        for (unsigned int i = 0; i < m_geometry->getNumVertices(); ++i) {
            //out vec3 eye_light_dir;
            //out vec3 eye_eye_dir;
            //out vec3 eye_reflected_dir;

            std::cout << "eye_light_dir[" << i << "]     = ("
                << feedback[9*i] << ", "
                << feedback[9*i+1] << ", "
                << feedback[9*i+2] << ")"
                << std::endl;
            std::cout << "eye_eye_dir[" << i << "]       = ("
                << feedback[9*i+3] << ", "
                << feedback[9*i+4] << ", "
                << feedback[9*i+5] << ")"
                << std::endl;
            std::cout << "eye_reflected_dir[" << i << "] = ("
                << feedback[9*i+6] << ", "
                << feedback[9*i+7] << ", "
                << feedback[9*i+8] << ")"
                << std::endl;
        }

        glUnmapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER);
    }
};
