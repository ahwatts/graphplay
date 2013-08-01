// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "Geometry.h"
#include "Collada.h"

namespace graphplay {
    Geometry::Geometry(void)
        : m_positions(),
          m_normals(),
          m_colors(),
          m_tex_coords(),
          m_indices(),
          m_data_buffer(0),
          m_element_buffer(0) { }

    Geometry::Geometry(const collada::MeshGeometry &mesh_geo)
        : m_positions(),
          m_normals(),
          m_colors(),
          m_tex_coords(),
          m_indices(),
          m_data_buffer(0),
          m_element_buffer(0) {
        for (collada::MeshGeometry::iterator v = mesh_geo.begin(); v != mesh_geo.end(); ++v) {
            collada::MeshGeometry::value_type vertex = *v;
            for (collada::MeshGeometry::value_type::iterator a = vertex.begin(); a != vertex.end(); ++a) {
                const std::string &attr_name = a->first;
                std::vector<float> &attr_value = a->second;
                if (attr_name == "VERTEX") {
                    m_positions.push_back(glm::vec3(attr_value[0], attr_value[1], attr_value[2]));
                } else if (attr_name == "NORMAL") {
                    m_normals.push_back(glm::vec3(attr_value[0], attr_value[1], attr_value[2]));
                } else if (attr_name == "COLOR") {
                    if (attr_value.size() == 3) {
                        m_colors.push_back(glm::vec4(attr_value[0], attr_value[1], attr_value[2], 0.0));
                    } else {
                        m_colors.push_back(glm::vec4(attr_value[0], attr_value[1], attr_value[2], attr_value[3)));
                    }
                } else if (attr_name == "TEXCOORD") {
                }
            }
        }
    }

    /* void Geometry::generateBuffers() {
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
        }*/
};
