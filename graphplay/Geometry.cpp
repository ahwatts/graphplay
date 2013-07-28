#include "Geometry.h"

namespace graphplay {
    Geometry::Geometry(void)
    { }

    Geometry::~Geometry(void)
    { }

    void Geometry::generateBuffers() {
        GLuint buffers[2];
        GLsizeiptr vertex_size = m_positions.size() * 3 * sizeof(float);
        GLsizeiptr color_size = m_colors.size() * 4 * sizeof(float);
        GLsizeiptr total_size = vertex_size + color_size;

        glGenBuffers(2, buffers);
        m_data_buffer = buffers[0];
        m_element_buffer = buffers[1];

        glBindBuffer(GL_ARRAY_BUFFER, m_data_buffer);
        glBufferData(GL_ARRAY_BUFFER, total_size, NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_size, m_positions.data());
        glBufferSubData(GL_ARRAY_BUFFER, vertex_size, color_size, m_colors.data());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_element_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint),
            m_indices.data(), GL_STATIC_DRAW);
    }

    void Geometry::render(Material &material) {
        GLuint position_location = material.getPositionLocation();
        GLuint color_location = material.getColorLocation();
        GLsizeiptr vertex_size = m_positions.size() * 3 * sizeof(float);

        glBindBuffer(GL_ARRAY_BUFFER, m_data_buffer);
        glEnableVertexAttribArray(position_location);
        glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)0);
        glEnableVertexAttribArray(color_location);
        glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_TRUE, 0, (const GLvoid *)vertex_size);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_element_buffer);
        glDrawElements(GL_TRIANGLES, m_indices.size() / 2, GL_UNSIGNED_INT, 0);
    }
};
