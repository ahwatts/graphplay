// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "Geometry.h"

namespace graphplay {
    Geometry::Geometry()
        : m_vertex_attrs(),
          m_vertex_elems(),
          m_position_offset(-1),
          m_normal_offset(-1),
          m_color_offset(-1),
          m_tex_coord_offset(-1),
          m_stride(0),
          m_new_vertex(),
          m_data_buffer(0),
          m_element_buffer(0) { }

    /*Geometry::Geometry(const collada::MeshGeometry &mesh_geo)
        : m_vertex_attrs(),
          m_vertex_elems(),
          m_position_offset(-1),
          m_normal_offset(-1),
          m_color_offset(-1),
          m_tex_coord_offset(-1),
          m_stride(0),
          m_new_vertex(),
          m_data_buffer(0),
          m_element_buffer(0) {
        loadFromCollada(mesh_geo);
    }*/

    /*void Geometry::loadFromCollada(const collada::MeshGeometry &mesh_geo) {
        std::map<const std::string, unsigned int> offsets;

        // Copy the vertex data into m_data_buffer.
        for (auto v : mesh_geo) {
            std::vector<float> vdata(m_stride);

            // First, copy this vertex's data into vdata, stashing off
            // the offsets for the different attributes if we haven't
            // already.
            for (auto a : v) {
                const std::string &attr_name = a.first;
                std::vector<float> &attr_value = a.second;


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
    }*/

    void Geometry::commitNewVertex() {
        if (m_new_vertex.size() == 0) return;

        unsigned int index = findVertex(m_new_vertex);

        if (index < m_vertex_elems.size()) {
            m_vertex_elems.push_back(index);
        } else {
            for (float v : m_new_vertex) { m_vertex_attrs.push_back(v); }
            m_vertex_elems.push_back(m_vertex_elems.size());
        }

        m_new_vertex = std::vector<float>();
    }

    unsigned int Geometry::findVertex(std::vector<float> &vdata) {
        for (unsigned int v_off = 0; v_off < m_vertex_attrs.size(); v_off += m_stride) {
            bool match = true;

            for (unsigned int i = 0; i < m_stride; ++i) {
                if (vdata[i] != m_vertex_attrs[v_off + i]) {
                    match = false;
                    break;
                }
            }

            if (match) {
                return v_off / m_stride;
            }
        }

        return m_vertex_elems.size();
    }

    void Geometry::vertex3f(float x, float y, float z) {
        commitNewVertex();
        m_new_vertex = std::vector<float>(m_stride);

        if (m_position_offset < 0 && m_vertex_elems.size() == 0) {
            m_position_offset = m_stride;
            m_stride += 3;
        }

        if (m_position_offset < 0) { return; }

        while (m_stride > m_new_vertex.size()) { m_new_vertex.push_back(0); }

        m_new_vertex[m_position_offset]     = x;
        m_new_vertex[m_position_offset + 1] = y;
        m_new_vertex[m_position_offset + 2] = z;
    }

    void Geometry::color4f(float r, float g, float b, float a) {
        if (m_color_offset < 0 && m_vertex_elems.size() == 0) {
            m_color_offset = m_stride;
            m_stride += 3;
        }

        if (m_color_offset < 0) { return; }

        while (m_stride > m_new_vertex.size()) { m_new_vertex.push_back(0); }

        m_new_vertex[m_color_offset]     = r;
        m_new_vertex[m_color_offset + 1] = g;
        m_new_vertex[m_color_offset + 2] = b;
        m_new_vertex[m_color_offset + 3] = a;
    }

    void Geometry::generateBuffers() {
        commitNewVertex();

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

    OctohedronGeometry::OctohedronGeometry() : Geometry() {
        vertex3f( 0,  0,  1); color4f(0, 0, 1, 1);
        vertex3f( 1,  0,  0); color4f(1, 0, 0, 1);
        vertex3f( 0,  1,  0); color4f(0, 1, 0, 1);

        vertex3f( 0,  0,  1); color4f(0, 0, 1, 1);
        vertex3f( 0,  1,  0); color4f(0, 1, 0, 1);
        vertex3f(-1,  0,  0); color4f(1, 0, 0, 1);

        vertex3f( 0,  0,  1); color4f(0, 0, 1, 1);
        vertex3f(-1,  0,  0); color4f(1, 0, 0, 1);
        vertex3f( 0, -1,  0); color4f(0, 1, 0, 1);
        
        vertex3f( 0,  0,  1); color4f(0, 0, 1, 1);
        vertex3f( 0, -1,  0); color4f(0, 1, 0, 1);
        vertex3f( 1,  0,  0); color4f(1, 0, 0, 1);
        
        vertex3f( 0,  0, -1); color4f(0, 0, 1, 1);
        vertex3f( 0,  1,  0); color4f(0, 1, 0, 1);
        vertex3f( 1,  0,  0); color4f(1, 0, 0, 1);
        
        vertex3f( 0,  0, -1); color4f(0, 0, 1, 1);
        vertex3f(-1,  0,  0); color4f(1, 0, 0, 1);
        vertex3f( 0,  1,  0); color4f(0, 1, 0, 1);
        
        vertex3f( 0,  0, -1); color4f(0, 0, 1, 1);
        vertex3f( 0, -1,  0); color4f(0, 1, 0, 1);
        vertex3f(-1,  0,  0); color4f(1, 0, 0, 1);
        
        vertex3f( 0,  0, -1); color4f(0, 0, 1, 1);
        vertex3f( 1,  0,  0); color4f(1, 0, 0, 1);
        vertex3f( 0, -1,  0); color4f(0, 1, 0, 1);

        commitNewVertex();
    }
};
