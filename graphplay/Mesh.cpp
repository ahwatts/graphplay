// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

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
    const char *DebugMesh::vertex_shader_1_src = GLSL(
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

    const char *DebugMesh::fragment_shader_1_src = GLSL(
        in vec4 vAmbientColor;
        in vec4 vDiffuseColor;
        in vec4 vSpecularColor;

        out vec4 FragColor;

        void main(void) {
            FragColor = clamp(vAmbientColor + vDiffuseColor + vSpecularColor, 0.0, 1.0);
        }
    );

    const char *DebugMesh::vertex_shader_2_src = GLSL(
        in vec3 aPosition;
        in vec3 aDirection;

        uniform mat4x4 uModelView;
        uniform mat4x4 uProjection;

        void main(void) {
            if (gl_VertexID % 2 == 0) {
                gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);
            } else {
                gl_Position = uProjection * uModelView * vec4(aPosition + 0.5*normalize(aDirection), 1.0);
            }
        }
    );

    // const char *DebugMesh::vertex_shader_2_src =
    //     "#version 430 core\n"
    //     "in vec3 aPosition;\n"
    //     "in vec3 aDirection;\n"
    //     "uniform mat4x4 uModelView;\n"
    //     "uniform mat4x4 uProjection;\n"
    //     "void main(void) {\n"
    //     "    if (gl_VertexID % 2 == 0) {\n"
    //     "        gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n"
    //     "    } else {\n"
    //     "        gl_Position = uProjection * uModelView * vec4(aPosition + 0.5*normalize(aDirection), 1.0);\n"
    //     "    }\n"
    //     "}\n";

    const char *DebugMesh::fragment_shader_2_src = GLSL(
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
        const char *varying_names[] = { "eye_light_dir", "eye_eye_dir", "eye_reflected_dir" };
        glTransformFeedbackVaryings(m_program_1, 3, varying_names, GL_INTERLEAVED_ATTRIBS);
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

        // Create the buffer to receive the transform feedback.
        glGenBuffers(1, &m_feedback_buffer);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m_feedback_buffer);
        glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER,
            3*3*sizeof(float)*m_geometry->getNumVertices(),
            NULL, GL_DYNAMIC_COPY);

        // Build a new element array for rendering the transform
        // feedback.
        GLuint *feedback_elements = new GLuint[m_geometry->getNumVertices()*2];
        GLuint *geometry_elements = NULL;
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_geometry->getElementArrayBuffer());
        geometry_elements = (GLuint *)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY);
        for (unsigned int i = 0; i < m_geometry->getNumVertices(); ++i) {
            // Add each vertex twice, once for each endpoint of
            // the line we'll draw.
            feedback_elements[2*i] = geometry_elements[i];
            feedback_elements[2*i+1] = geometry_elements[i];            
        }
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

        // Create an element array buffer for rendering the transform
        // feedback.
        glGenBuffers(1, &m_feedback_element_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_feedback_element_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            m_geometry->getNumVertices()*2*sizeof(GLuint),
            feedback_elements,
            GL_STATIC_DRAW);

        // All that data's now on the GPU; we don't need it here any more.
        delete [] feedback_elements;

        // Build the second-pass shader program.
        vertex_shader = createAndCompileShader(GL_VERTEX_SHADER, DebugMesh::vertex_shader_2_src);
        fragment_shader = createAndCompileShader(GL_FRAGMENT_SHADER, DebugMesh::fragment_shader_2_src);
        m_program_2 = createProgramFromShaders(vertex_shader, fragment_shader);

        m_position_loc_2 = (GLuint)glGetAttribLocation(m_program_2, "aPosition");
        m_direction_loc_2 = (GLuint)glGetAttribLocation(m_program_2, "aDirection");
        m_projection_loc_2 = glGetUniformLocation(m_program_2, "uProjection");
        m_model_view_loc_2 = glGetUniformLocation(m_program_2, "uModelView");
    }

    void DebugMesh::render(const glm::mat4x4 &projection, const glm::mat4x4 &model_view) const {
        glm::mat4x4 full_mv = model_view * m_model_transform;
        glm::mat3x3 mv_inverse = glm::inverseTranspose(glm::mat3x3(full_mv));
        glm::vec3 light_pos(0, 0, 10);
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

		glBindBuffer(GL_ARRAY_BUFFER, m_geometry->getArrayBuffer());
		glEnableVertexAttribArray(m_position_loc_2);
		glVertexAttribPointer(m_position_loc_2, 3, GL_FLOAT, GL_FALSE, vertex_size,
			BUFFER_OFFSET_BYTES(m_position_loc*sizeof(float)));

		glBindBuffer(GL_ARRAY_BUFFER, m_feedback_buffer);
		glEnableVertexAttribArray(m_direction_loc_2);
		glVertexAttribPointer(m_direction_loc_2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
			BUFFER_OFFSET_BYTES(0));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_feedback_element_buffer);

		glDrawElements(GL_LINES, m_geometry->getNumVertices(), GL_UNSIGNED_INT, 0);
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
