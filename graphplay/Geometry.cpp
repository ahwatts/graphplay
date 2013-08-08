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

                if (m_offsets.find(attr_name) == m_offsets.end()) {
                    m_offsets[attr_name] = vdata.size();
                    m_stride += attr_value.size();
                    if (attr_name == "COLOR" && attr_value.size() == 3) {
                        m_stride += 1;
                    }
                }

                unsigned int offset = m_offsets[attr_name];
                unsigned int new_size = vdata.size() + attr_value.size();

                for (unsigned int i = 0; i < new_size; ++i) {
                    if (i >= offset && i < offset + attr_value.size()) {
                        float val = attr_value[i - offset];
                        if (i >= vdata.size()) {
                            vdata.push_back(val);
                        } else {
                            vdata[i] = val;
                        }
                    }
                }

                if (attr_name == "COLOR" && attr_value.size() == 3) {
                    unsigned int a_off = offset + 3;
                    if (a_off >= vdata.size()) {
                        vdata.push_back(0);
                    } else {
                        vdata[a_off] = 0.0;
                    }
                }
            }

            bool match = false;
            for (unsigned int v_off = 0; v_off < m_vertex_attrs.size(); v_off += m_stride) {
                bool this_matches = true;
                for (unsigned int i = 0; i < m_stride; ++i) {
                    if (vdata[i] != m_vertex_attrs[v_off + i]) {
                        this_matches = false;
                        break;
                    }
                }

                if (this_matches) {
                    m_vertex_elems.push_back(v_off / m_stride);
                    match = true;
                    break;
                }
            }

            if (!match) {
                m_vertex_elems.push_back(m_vertex_attrs.size() / m_stride);
                for (unsigned int i = 0; i < vdata.size(); ++i) {
                    m_vertex_attrs.push_back(vdata[i]);
                }
            }
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
