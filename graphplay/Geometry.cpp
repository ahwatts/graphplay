// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "Geometry.h"
#include "Collada.h"

#include <iostream>

namespace graphplay {
    Geometry::Geometry(void)
        : m_vertex_attrs(),
          m_vertex_elems(),
          m_offsets(),
          m_stride(0),
          m_data_buffer(0),
          m_element_buffer(0) { }

    Geometry::Geometry(const collada::MeshGeometry &mesh_geo)
        : m_vertex_attrs(),
          m_vertex_elems(),
          m_offsets(),
          m_stride(0),
          m_data_buffer(0),
          m_element_buffer(0) {
        for (collada::MeshGeometry::iterator v = mesh_geo.begin(); v != mesh_geo.end(); ++v) {
            collada::MeshGeometry::value_type vertex = *v;
            std::vector<float> vdata(m_stride);

            for (collada::MeshGeometry::value_type::iterator a = vertex.begin(); a != vertex.end(); ++a) {
                const std::string &attr_name = a->first;
                std::vector<float> &attr_value = a->second;

                std::cout << attr_name << ": ";
                for (unsigned int i = 0; i < attr_value.size(); ++i) std::cout << attr_value[i] << ", ";
                std::cout << std::endl;

                if (m_offsets.find(attr_name) == m_offsets.end()) {
                    m_offsets[attr_name] = attr_value.size();
                    m_stride += attr_value.size();
                    if (attr_name == "COLOR" && attr_value.size() == 3) {
                        m_stride += 1;
                    }
                    std::cout << "  m_offsets[" << attr_name << "] = " << m_offsets[attr_name] << std::endl;
                    std::cout << "  m_stride = " << m_stride << std::endl;
                }

                unsigned int offset = m_offsets[attr_name];
                vdata.reserve(vdata.size() + attr_value.size());

                std::cout << "  stride = " << m_stride << " offset = " << offset << " vdata.size() = " << vdata.size() << std::endl;

                for (unsigned int i = 0; i < attr_value.size(); ++i) {
                    vdata[offset + i] = attr_value[i];
                }

                if (attr_name == "COLOR" && attr_value.size() == 3) {
                    vdata.reserve(vdata.size() + 1);
                    vdata[offset + 3] = 0.0;
                }

                std::cout << "  vdata = { ";
                for (unsigned int i = 0; i < vdata.size(); ++i) {
                    std::cout << vdata[i] << ", ";
                }
                std::cout << " }" << std::endl << std::endl;
            }

            break;

        }
    }

    /* void Geometry::generateBuffers() {
        GLuint buffers[2];
        GLsizeiptr vertex_size = m_positions.size() * 3 * sizeof(float);
        GLsizeiptr normal_size = m_normals.size() * 3 * sizeof(float);
        GLsizeiptr color_size = m_colors.size() * 4 * sizeof(float);
        GLsizeiptr tex_coord_size = m_tex_coords.size() * 2 * sizeof(float);
        GLsizeiptr total_size = vertex_size + normal_size + color_size + tex_coord_size;

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
    } */
};
