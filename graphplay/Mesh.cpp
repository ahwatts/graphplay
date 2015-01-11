// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "graphplay.h"
#include "Mesh.h"
#include "Material.h"
#include "graphplay.h"

#include <iostream>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>

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
        m_geometry->setUpVertexArray(*m_material);
    }

    void Mesh::setGeometry(sp_Geometry geo) {
        m_geometry = geo;

        if (m_material) {
            m_geometry->setUpVertexArray(*m_material);
        }
    }

    void Mesh::setMaterial(sp_Material mat) {
        m_material = mat;

        if (m_geometry) {
            m_geometry->setUpVertexArray(*m_material);
        }
    }

    void Mesh::setTransform(const glm::mat4x4 &new_transform) {
        copy_mat4x4_to_array(m_model_transform, new_transform);
    }

    void Mesh::render(const glm::mat4x4 &projection, const glm::mat4x4 &model_view) const {
        glm::mat4x4 transform_mat = glm::make_mat4x4(m_model_transform);
        glm::mat4x4 full_mv = model_view * transform_mat;
        m_geometry->render(projection, full_mv, *m_material);
    }

    // Class DebugMesh.
    const char *DebugMesh::vertex_shader_1_src = R"glsl(
        #version 410 core

        in vec3 aPosition;
        in vec3 aNormal;
        in vec4 aColor;

        uniform mat4x4 uModel;
        uniform mat3x3 uModel3InverseTranspose;
        uniform mat4x4 uView;
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
            vec4 wld_vert_position4 = uModel * vec4(aPosition, 1.0);
            vec3 wld_vert_position = (1.0 / wld_vert_position4.w) * vec3(wld_vert_position4);
            vec3 wld_vert_normal = normalize(uModel3InverseTranspose * aNormal);
            vec3 wld_vert_light_dir = normalize(uLightPosition - wld_vert_position);

            vec4 eye_vert_position4 = uView * wld_vert_position4;
            vec3 eye_vert_position = (1.0 / eye_vert_position4.w) * vec3(eye_vert_position4);
            vec4 eye_light_position4 = uView * vec4(uLightPosition, 1.0);
            vec3 eye_light_position = (1.0 / eye_light_position4.w) * vec3(eye_light_position4);
            vec3 eye_vert_light_dir = normalize(eye_light_position - eye_vert_position);
            vec3 eye_vert_eye_dir = normalize(-1*eye_vert_position);

            light_src = eye_vert_position;
            light_dst = eye_vert_position + 0.5*eye_vert_light_dir;

            eye_src = eye_vert_position;
            eye_dst = eye_vert_position * 0.5*eye_vert_eye_dir;

            // Make this be nothing for now.
            reflected_src = eye_vert_position;
            reflected_dst = eye_vert_position;

            gl_Position = uProjection * eye_vert_position4;

            vAmbientColor = 0.5 * aColor;
            // vAmbientColor = vec4(0.0, 0.0, 0.0, 1.0);

            vDiffuseColor = 0.5 * dot(wld_vert_normal, wld_vert_light_dir) * (uLightColor * aColor);
            // vDiffuseColor = vec4(0.0, 0.0, 0.0, 1.0);

            // Make this be nothing for now.
            vSpecularColor = vec4(0.0, 0.0, 0.0, 1.0);
        }
    )glsl";

    const char *DebugMesh::fragment_shader_1_src = R"glsl(
        #version 410 core

        in vec4 vAmbientColor;
        in vec4 vDiffuseColor;
        in vec4 vSpecularColor;

        out vec4 FragColor;

        void main(void) {
            if (gl_FrontFacing) {
                FragColor = clamp(vAmbientColor + vDiffuseColor + vSpecularColor, 0.0, 1.0);
            } else {
                FragColor = vec4(0.0, 0.0, 0.0, 1.0);
            }
        }
    )glsl";

    const char *DebugMesh::vertex_shader_2_src = R"glsl(
        #version 410 core

        in vec3 aPosition;

        uniform mat4x4 uProjection;

        out vec4 vColor;

        void main(void) {
            gl_Position = uProjection * vec4(aPosition, 1.0);
            vColor = gl_VertexID % 2 == 0 ? vec4(1.0, 1.0, 0.0, 1.0) : vec4(1.0, 0.0, 1.0, 1.0);
        }
    )glsl";

    const char *DebugMesh::fragment_shader_2_src = R"glsl(
        #version 410 core

        in vec4 vColor;

        out vec4 FragColor;

        void main(void) {
            FragColor = vColor;
        }
    )glsl";

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

        m_model_loc = glGetUniformLocation(m_program_1, "uModel");
        m_model_3_inv_trans_loc = glGetUniformLocation(m_program_1, "uModel3InverseTranspose");
        m_view_loc = glGetUniformLocation(m_program_1, "uView");
        m_projection_loc = glGetUniformLocation(m_program_1, "uProjection");
        m_light_position_loc = glGetUniformLocation(m_program_1, "uLightPosition");
        m_light_color_loc = glGetUniformLocation(m_program_1, "uLightColor");
        m_specular_exponent_loc = glGetUniformLocation(m_program_1, "uSpecularExponent");

        // Create our VAOs.
        GLuint vaos[2];
        glGenVertexArrays(2, vaos);
        m_vao_1 = vaos[0];
        m_vao_2 = vaos[1];

        // Set up our first-pass VAO.
        if (!m_geometry->arrayAndBuffersCreated()) m_geometry->createArrayAndBuffers();
        GLsizeiptr vertex_size = m_geometry->getStride() * sizeof(float);
        glBindVertexArray(m_vao_1);
        glBindBuffer(GL_ARRAY_BUFFER, m_geometry->getArrayBuffer());

        glEnableVertexAttribArray(m_position_loc);
        glVertexAttribPointer(m_position_loc, 3, GL_FLOAT, GL_FALSE, vertex_size,
            BUFFER_OFFSET_BYTES(m_geometry->getPositionOffset()*sizeof(float)));

        glEnableVertexAttribArray(m_normal_loc);
        glVertexAttribPointer(m_normal_loc, 3, GL_FLOAT, GL_FALSE, vertex_size,
            BUFFER_OFFSET_BYTES(m_geometry->getNormalOffset()*sizeof(float)));

        glEnableVertexAttribArray(m_color_loc);
        glVertexAttribPointer(m_color_loc, 4, GL_FLOAT, GL_FALSE, vertex_size,
            BUFFER_OFFSET_BYTES(m_geometry->getColorOffset()*sizeof(float)));        

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Create the buffer to receive the transform feedback. It will contain six vec3's per vertex.
        unsigned int num_geo_verts = m_geometry->getNumVertices();
        glGenBuffers(1, &m_feedback_buffer);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m_feedback_buffer);
        glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, 6*3*num_geo_verts*sizeof(float), NULL, GL_DYNAMIC_COPY);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

        // Build the second-pass shader program.
        vertex_shader = createAndCompileShader(GL_VERTEX_SHADER, DebugMesh::vertex_shader_2_src);
        fragment_shader = createAndCompileShader(GL_FRAGMENT_SHADER, DebugMesh::fragment_shader_2_src);
        m_program_2 = createProgramFromShaders(vertex_shader, fragment_shader);

        m_position_loc_2 = (GLuint)glGetAttribLocation(m_program_2, "aPosition");
        m_projection_loc_2 = glGetUniformLocation(m_program_2, "uProjection");

        // Set up our second-pass VAO.
        glBindVertexArray(m_vao_2);
        glBindBuffer(GL_ARRAY_BUFFER, m_feedback_buffer);

        glEnableVertexAttribArray(m_position_loc_2);
        glVertexAttribPointer(m_position_loc_2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), BUFFER_OFFSET_BYTES(0*sizeof(float)));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void DebugMesh::render(const glm::mat4 &projection, const glm::mat4 &view) const {
        glm::mat4 model = glm::make_mat4(m_model_transform);
        glm::mat3 model_3_inv_trans = glm::inverseTranspose(glm::mat3(model));
        glm::vec3 light_pos(0, 10, 0);
        glm::vec4 light_color(0.5, 1.0, 0.75, 1.0);
        unsigned int specular_exponent = 2;

        // Use our special shader program.
        glUseProgram(m_program_1);

        // Set the inputs for the uniforms.
        glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix3fv(m_model_3_inv_trans_loc, 1, GL_FALSE, glm::value_ptr(model_3_inv_trans));
        glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(m_projection_loc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(m_light_position_loc, 1, glm::value_ptr(light_pos));
        glUniform4fv(m_light_color_loc, 1, glm::value_ptr(light_color));
        glUniform1ui(m_specular_exponent_loc, specular_exponent);

        // Bind what we want.
        glBindVertexArray(m_vao_1);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_geometry->getElementArrayBuffer());
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_feedback_buffer);

        // Render with transform feedback.
        glBeginTransformFeedback(m_geometry->getDrawType());
        glDrawElements(m_geometry->getDrawType(), m_geometry->getNumVertices(), GL_UNSIGNED_INT, 0);
        glEndTransformFeedback();

        // Clean up.
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

        // Render the feedback.
        glUseProgram(m_program_2);
        glUniformMatrix4fv(m_projection_loc_2, 1, GL_FALSE, glm::value_ptr(projection));
        glBindVertexArray(m_vao_2);
        glDrawArrays(GL_LINES, 0, 2*m_geometry->getNumVertices());

        // Clean up.
        glBindVertexArray(0);
        glUseProgram(0);

        // printTransformFeedback();
    }

    void DebugMesh::printTransformFeedback() const {
        float *feedback = NULL;
        
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m_feedback_buffer);
        feedback = (float*)glMapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, GL_READ_ONLY);

        if (!feedback) {
            std::cout << "Could not map feedback buffer!" << std::endl;
            return;
        }
        
        for (unsigned int i = 0; i < 1; ++i) {
            //out vec3 light_src;
            //out vec3 eye_src;
            //out vec3 reflected_src;
            //out vec3 light_dst;
            //out vec3 eye_dst;
            //out vec3 reflected_dst;

            glm::vec3 light_src(feedback[18*i+0], feedback[18*i+ 1], feedback[18*i+ 2]);
            glm::vec3 light_dst(feedback[18*i+9], feedback[18*i+10], feedback[18*i+11]);
            glm::vec3 eye_src(feedback[18*i+ 3], feedback[18*i+ 4], feedback[18*i+ 5]);
            glm::vec3 eye_dst(feedback[18*i+12], feedback[18*i+13], feedback[18*i+14]);
            glm::vec3 reflected_src(feedback[18*i+ 6], feedback[18*i+ 7], feedback[18*i+ 8]);
            glm::vec3 reflected_dst(feedback[18*i+15], feedback[18*i+16], feedback[18*i+17]);

            std::cout << "light: " << light_src << " -> " << light_dst << " (" << light_dst - light_src << ")" << std::endl;
        }

        glUnmapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER);
    }
};
