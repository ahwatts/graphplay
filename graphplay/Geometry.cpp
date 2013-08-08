// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "Geometry.h"
#include "Collada.h"

#include <iostream>

namespace graphplay {
    Geometry::Geometry(void)
        : m_vertex_attrs(),
          m_vertex_elems(),
          m_position_offset(-1),
          m_normal_offset(-1),
          m_color_offset(-1),
          m_tex_coord_offset(-1),
          m_stride(0),
          m_data_buffer(0),
          m_element_buffer(0) { }

    Geometry::Geometry(const collada::MeshGeometry &mesh_geo)
        : m_vertex_attrs(),
          m_vertex_elems(),
          m_position_offset(-1),
          m_normal_offset(-1),
          m_color_offset(-1),
          m_tex_coord_offset(-1),
          m_stride(0),
          m_data_buffer(0),
          m_element_buffer(0) {
        std::map<const std::string, unsigned int> offsets;

        // Copy the vertex data into m_data_buffer.
        for (collada::MeshGeometry::iterator v = mesh_geo.begin(); v != mesh_geo.end(); ++v) {
            collada::MeshGeometry::value_type vertex = *v;
            std::vector<float> vdata(m_stride);

            // First, copy this vertex's data into vdata, stashing off
            // the offsets for the different attributes if we haven't
            // already.
            for (collada::MeshGeometry::value_type::iterator a = vertex.begin(); a != vertex.end(); ++a) {
                const std::string &attr_name = a->first;
                std::vector<float> &attr_value = a->second;

                if (offsets.find(attr_name) == offsets.end()) {
                    offsets[attr_name] = vdata.size();
                    m_stride += attr_value.size();
                    if (attr_name == "COLOR" && attr_value.size() == 3) {
                        m_stride += 1;
                    }
                }

                unsigned int offset = offsets[attr_name];
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

            // Now, search through the vertices we've already read to
            // see if we've already read a vertex with this vertex's
            // data. If so, don't add it to m_vertex_data, just store
            // the original vertex's index to m_vertex_elems.
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

            // If no match was found, add vdata to m_vertex_data and
            // its index to m_vertex_elems.
            if (!match) {
                m_vertex_elems.push_back(m_vertex_attrs.size() / m_stride);
                for (unsigned int i = 0; i < vdata.size(); ++i) {
                    m_vertex_attrs.push_back(vdata[i]);
                }
            }
        }

        // Finally, store off the offsets we've been using into the
        // appropriate member variables.
        for (std::map<const std::string, unsigned int>::iterator off = offsets.begin(); off != offsets.end(); ++off) {
            const std::string &semantic = off->first;

            if (semantic == "VERTEX") {
                m_position_offset = off->second;
            } else if (semantic == "NORMAL") {
                m_normal_offset = off->second;
            } else if (semantic == "COLOR") {
                m_color_offset = off->second;
            } else if (semantic == "TEXCOORD") {
                m_tex_coord_offset = off->second;
            }
        }
    }

    void Geometry::generateBuffers() {
        GLuint buffers[2];

        glGenBuffers(2, buffers);
        m_data_buffer = buffers[0];
        m_element_buffer = buffers[1];

        glBindBuffer(GL_ARRAY_BUFFER, m_data_buffer);
        glBufferData(GL_ARRAY_BUFFER,
                     m_vertex_attrs.size() * sizeof(float),
                     m_vertex_attrs.data(),
                     GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_element_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     m_vertex_elems.size() * sizeof(GLuint),
                     m_vertex_elems.data(),
                     GL_STATIC_DRAW);
    }

    /* void Geometry::render(Material &material) {
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
