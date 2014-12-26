// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

// #include <glm/glm.hpp>
// #include <glm/gtc/type_ptr.hpp>

#include "Geometry.h"

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Material.h"
#include "graphplay.h"

namespace graphplay {
    Geometry::Geometry()
        : m_draw_type(GL_TRIANGLES),
          m_vertex_attrs(),
          m_vertex_elems(),
          m_position_offset(-1),
          m_normal_offset(-1),
          m_color_offset(-1),
          m_tex_coord_offset(-1),
          m_stride(0),
          m_new_vertex(),
          m_new_vertex_started(false),
          m_data_buffer(0),
          m_element_buffer(0),
          m_buffers_created(false) { }

    Geometry::~Geometry() {
        if (m_buffers_created) {
            destroyBuffers();
        }
    }

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
        if (!m_new_vertex_started || m_position_offset < 0) return;

        unsigned int index = findVertex(m_new_vertex);

        if (index < m_vertex_elems.size()) {
            m_vertex_elems.push_back(index);
        } else {
            unsigned int new_index = m_vertex_attrs.size() / m_stride;
            for (float v : m_new_vertex) { m_vertex_attrs.push_back(v); }
            m_vertex_elems.push_back(new_index);
        }

        m_new_vertex = std::vector<float>(m_stride);
        m_new_vertex_started = false;
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

        if (m_position_offset < 0 && m_vertex_elems.size() == 0) {
            m_position_offset = m_stride;
            m_stride += 3;
        }

        if (m_position_offset < 0) { return; }

        while (m_stride > m_new_vertex.size()) { m_new_vertex.push_back(0); }

        m_new_vertex[m_position_offset]     = x;
        m_new_vertex[m_position_offset + 1] = y;
        m_new_vertex[m_position_offset + 2] = z;
        m_new_vertex_started = true;
    }

    void Geometry::normal3f(float x, float y, float z) {
        if (m_normal_offset < 0 && m_vertex_elems.size() == 0) {
            m_normal_offset = m_stride;
            m_stride += 3;
        }

        if (m_normal_offset < 0) { return; }

        while (m_stride > m_new_vertex.size()) { m_new_vertex.push_back(0); }

        m_new_vertex[m_normal_offset]     = x;
        m_new_vertex[m_normal_offset + 1] = y;
        m_new_vertex[m_normal_offset + 2] = z;
        m_new_vertex_started = true;
    }

    void Geometry::color4f(float r, float g, float b, float a) {
        if (m_color_offset < 0 && m_vertex_elems.size() == 0) {
            m_color_offset = m_stride;
            m_stride += 4;
        }

        if (m_color_offset < 0) { return; }

        while (m_stride > m_new_vertex.size()) { m_new_vertex.push_back(0); }

        m_new_vertex[m_color_offset]     = r;
        m_new_vertex[m_color_offset + 1] = g;
        m_new_vertex[m_color_offset + 2] = b;
        m_new_vertex[m_color_offset + 3] = a;
        m_new_vertex_started = true;
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

        m_buffers_created = true;
    }

    void Geometry::destroyBuffers() {
        if (m_buffers_created) {
            if (glIsBuffer(m_data_buffer)) {
                glDeleteBuffers(1, &m_data_buffer);
            }

            if (glIsBuffer(m_element_buffer)) {
                glDeleteBuffers(1, &m_element_buffer);
            }
        }
    }

    void Geometry::render(const glm::mat4x4 &projection, const glm::mat4x4 &model_view, const  Material &material) const {
        glm::mat3x3 mv_inverse = glm::inverseTranspose(glm::mat3x3(model_view));

        GLint pos_loc = material.getPositionLocation();
        GLint norm_loc = material.getNormalLocation();
        GLint color_loc = material.getColorLocation();
        GLint tc_loc = material.getTexCoordLocation();

        GLint proj_loc = material.getProjectionLocation();
        GLint mv_loc = material.getModelViewLocation();
        GLint mvi_loc = material.getModelViewInverseLocation();

        GLsizeiptr vertex_size = m_stride * sizeof(float);

        glUseProgram(material.getProgram());

        glBindBuffer(GL_ARRAY_BUFFER, m_data_buffer);

        if (m_position_offset >= 0 && pos_loc >= 0) {
            glEnableVertexAttribArray(pos_loc);
            glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, vertex_size, BUFFER_OFFSET_BYTES(m_position_offset*sizeof(float)));
        }

        if (m_normal_offset >= 0 && norm_loc >= 0) {
            glEnableVertexAttribArray(norm_loc);
            glVertexAttribPointer(norm_loc, 3, GL_FLOAT, GL_FALSE, vertex_size, BUFFER_OFFSET_BYTES(m_normal_offset*sizeof(float)));
        }

        if (color_loc >= 0) {
            if (m_color_offset >= 0) {
                glEnableVertexAttribArray(color_loc);
                glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_TRUE, vertex_size, BUFFER_OFFSET_BYTES(m_color_offset*sizeof(float)));
            } else {
                glDisableVertexAttribArray(color_loc);
                glVertexAttrib4f(color_loc, 1.0f, 1.0f, 1.0f, 1.0f);
            }
        }

        if (m_tex_coord_offset >= 0 && tc_loc >= 0) {
            glEnableVertexAttribArray(tc_loc);
            glVertexAttribPointer(tc_loc, 2, GL_FLOAT, GL_TRUE, vertex_size, BUFFER_OFFSET_BYTES(m_tex_coord_offset*sizeof(float)));
        }

        glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(mv_loc, 1, GL_FALSE, glm::value_ptr(model_view));
        glUniformMatrix3fv(mvi_loc, 1, GL_FALSE, glm::value_ptr(mv_inverse));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_element_buffer);
        glDrawElements(m_draw_type, m_vertex_elems.size(), GL_UNSIGNED_INT, 0);
    }

    Geometry::VertexIterator Geometry::begin() const {
        return Geometry::VertexIterator(*this, 0);
    }

    Geometry::VertexIterator Geometry::end() const {
        return Geometry::VertexIterator(*this, this->getNumVertices());
    }

    Geometry::VertexIterator::VertexIterator(const Geometry &geo, unsigned int init_loc)
        : m_geo(geo),
          m_loc(init_loc) { }

    bool Geometry::VertexIterator::operator==(const Geometry::VertexIterator &other) const {
        return !(*this != other);
    }

    bool Geometry::VertexIterator::operator!=(const Geometry::VertexIterator &other) const {
        return &m_geo != &other.m_geo || m_loc != other.m_loc;
    }

    Geometry::value_type Geometry::VertexIterator::operator*() {
        std::vector<float> rv(m_geo.m_stride);
        unsigned int vindex = m_geo.m_vertex_elems[m_loc];
        unsigned int base_offset = vindex*m_geo.m_stride;

        for (unsigned int i = 0; i < m_geo.m_stride; ++i) {
            rv[i] = m_geo.m_vertex_attrs[base_offset + i];
        }

        return rv;
    }

    Geometry::VertexIterator &Geometry::VertexIterator::operator++() {
        ++m_loc;
        return *this;
    }

    Geometry::VertexIterator Geometry::VertexIterator::operator++(int) {
        Geometry::VertexIterator clone(*this);
        ++m_loc;
        return clone;
    }

    OctohedronGeometry::OctohedronGeometry() : Geometry() {
        vertex3f( 0,  0,  1); color4f(0, 0, 1, 1); normal3f( 0.577f,  0.577f,  0.577f);
        vertex3f( 1,  0,  0); color4f(1, 0, 0, 1); normal3f( 0.577f,  0.577f,  0.577f);
        vertex3f( 0,  1,  0); color4f(0, 1, 0, 1); normal3f( 0.577f,  0.577f,  0.577f);

        vertex3f( 0,  0,  1); color4f(0, 0, 1, 1); normal3f(-0.577f,  0.577f,  0.577f);
        vertex3f( 0,  1,  0); color4f(0, 1, 0, 1); normal3f(-0.577f,  0.577f,  0.577f);
        vertex3f(-1,  0,  0); color4f(1, 0, 0, 1); normal3f(-0.577f,  0.577f,  0.577f);

        vertex3f( 0,  0,  1); color4f(0, 0, 1, 1); normal3f(-0.577f, -0.577f,  0.577f);
        vertex3f(-1,  0,  0); color4f(1, 0, 0, 1); normal3f(-0.577f, -0.577f,  0.577f);
        vertex3f( 0, -1,  0); color4f(0, 1, 0, 1); normal3f(-0.577f, -0.577f,  0.577f);

        vertex3f( 0,  0,  1); color4f(0, 0, 1, 1); normal3f( 0.577f, -0.577f,  0.577f);
        vertex3f( 0, -1,  0); color4f(0, 1, 0, 1); normal3f( 0.577f, -0.577f,  0.577f);
        vertex3f( 1,  0,  0); color4f(1, 0, 0, 1); normal3f( 0.577f, -0.577f,  0.577f);

        vertex3f( 0,  0, -1); color4f(0, 0, 1, 1); normal3f( 0.577f,  0.577f, -0.577f);
        vertex3f( 0,  1,  0); color4f(0, 1, 0, 1); normal3f( 0.577f,  0.577f, -0.577f);
        vertex3f( 1,  0,  0); color4f(1, 0, 0, 1); normal3f( 0.577f,  0.577f, -0.577f);

        vertex3f( 0,  0, -1); color4f(0, 0, 1, 1); normal3f(-0.577f,  0.577f, -0.577f);
        vertex3f(-1,  0,  0); color4f(1, 0, 0, 1); normal3f(-0.577f,  0.577f, -0.577f);
        vertex3f( 0,  1,  0); color4f(0, 1, 0, 1); normal3f(-0.577f,  0.577f, -0.577f);

        vertex3f( 0,  0, -1); color4f(0, 0, 1, 1); normal3f(-0.577f, -0.577f, -0.577f);
        vertex3f( 0, -1,  0); color4f(0, 1, 0, 1); normal3f(-0.577f, -0.577f, -0.577f);
        vertex3f(-1,  0,  0); color4f(1, 0, 0, 1); normal3f(-0.577f, -0.577f, -0.577f);

        vertex3f( 0,  0, -1); color4f(0, 0, 1, 1); normal3f( 0.577f, -0.577f, -0.577f);
        vertex3f( 1,  0,  0); color4f(1, 0, 0, 1); normal3f( 0.577f, -0.577f, -0.577f);
        vertex3f( 0, -1,  0); color4f(0, 1, 0, 1); normal3f( 0.577f, -0.577f, -0.577f);

        commitNewVertex();
    }

    CubeGeometry::CubeGeometry() : Geometry() {
        // Bottom face.
        vertex3f(0, 0, 0); color4f(0, 0, 0, 1); normal3f( 0,  0, -1);
        vertex3f(0, 1, 0); color4f(0, 1, 0, 1); normal3f( 0,  0, -1);
        vertex3f(1, 1, 0); color4f(1, 1, 0, 1); normal3f( 0,  0, -1);

        vertex3f(0, 0, 0); color4f(0, 0, 0, 1); normal3f( 0,  0, -1);
        vertex3f(1, 1, 0); color4f(1, 1, 0, 1); normal3f( 0,  0, -1);
        vertex3f(1, 0, 0); color4f(1, 0, 0, 1); normal3f( 0,  0, -1);

        // Top face.
        vertex3f(0, 0, 1); color4f(0, 0, 1, 1); normal3f( 0,  0,  1);
        vertex3f(1, 1, 1); color4f(1, 1, 1, 1); normal3f( 0,  0,  1);
        vertex3f(0, 1, 1); color4f(0, 1, 1, 1); normal3f( 0,  0,  1);

        vertex3f(0, 0, 1); color4f(0, 0, 1, 1); normal3f( 0,  0,  1);
        vertex3f(1, 0, 1); color4f(1, 0, 1, 1); normal3f( 0,  0,  1);
        vertex3f(1, 1, 1); color4f(1, 1, 1, 1); normal3f( 0,  0,  1);

        // Front face.
        vertex3f(0, 0, 0); color4f(0, 0, 0, 1); normal3f(-1,  0,  0);
        vertex3f(0, 0, 1); color4f(0, 0, 1, 1); normal3f(-1,  0,  0);
        vertex3f(0, 1, 1); color4f(0, 1, 1, 1); normal3f(-1,  0,  0);

        vertex3f(0, 0, 0); color4f(0, 0, 0, 1); normal3f(-1,  0,  0);
        vertex3f(0, 1, 1); color4f(0, 1, 1, 1); normal3f(-1,  0,  0);
        vertex3f(0, 1, 0); color4f(0, 1, 0, 1); normal3f(-1,  0,  0);

        // Back face.
        vertex3f(1, 0, 0); color4f(1, 0, 0, 1); normal3f( 1,  0,  0);
        vertex3f(1, 1, 0); color4f(1, 1, 0, 1); normal3f( 1,  0,  0);
        vertex3f(1, 1, 1); color4f(1, 1, 1, 1); normal3f( 1,  0,  0);

        vertex3f(1, 0, 0); color4f(1, 0, 0, 1); normal3f( 1,  0,  0);
        vertex3f(1, 1, 1); color4f(1, 1, 1, 1); normal3f( 1,  0,  0);
        vertex3f(1, 0, 1); color4f(1, 0, 1, 1); normal3f( 1,  0,  0);

        // Right face.
        vertex3f(0, 0, 0); color4f(0, 0, 0, 1); normal3f( 0, -1,  0);
        vertex3f(1, 0, 0); color4f(1, 0, 0, 1); normal3f( 0, -1,  0);
        vertex3f(1, 0, 1); color4f(1, 0, 1, 1); normal3f( 0, -1,  0);

        vertex3f(0, 0, 0); color4f(0, 0, 0, 1); normal3f( 0, -1,  0);
        vertex3f(1, 0, 1); color4f(1, 0, 1, 1); normal3f( 0, -1,  0);
        vertex3f(0, 0, 1); color4f(0, 0, 1, 1); normal3f( 0, -1,  0);

        // Left face.
        vertex3f(0, 1, 0); color4f(0, 1, 0, 1); normal3f( 0,  1,  0);
        vertex3f(1, 1, 0); color4f(1, 1, 0, 1); normal3f( 0,  1,  0);
        vertex3f(1, 1, 1); color4f(1, 1, 1, 1); normal3f( 0,  1,  0);

        vertex3f(0, 1, 0); color4f(0, 1, 0, 1); normal3f( 0,  1,  0);
        vertex3f(1, 1, 1); color4f(1, 1, 1, 1); normal3f( 0,  1,  0);
        vertex3f(0, 1, 1); color4f(0, 1, 1, 1); normal3f( 0,  1,  0);
    }

    NormalGeometry::NormalGeometry(const Geometry &base) {
        m_draw_type = GL_LINES;
        int poff = base.getPositionOffset();
        int noff = base.getNormalOffset();

        for (auto v : base) {
            glm::vec3 norm(v[noff], v[noff + 1], v[noff + 2]);
            glm::vec3 pos(v[poff], v[poff + 1], v[poff + 2]);
            glm::vec3 pos2;

            norm = glm::normalize(norm);
            pos2 = pos + (norm * 0.5f);
            vertex3f(pos.x, pos.y, pos.z);
            vertex3f(pos2.x, pos2.y, pos2.z);
        }
    }
};
