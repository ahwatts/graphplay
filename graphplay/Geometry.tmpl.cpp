// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

/* #include "graphplay.h"
#include "Geometry.h"

#include <iostream>
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>

#include "Material.h"

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
          m_vao(0),
          m_data_buffer(0),
          m_element_buffer(0),
          m_buffers_created(false),
          m_vao_initialized(false) { }

    Geometry::~Geometry() {
        destroyArrayAndBuffers();
    }

    / *Geometry::Geometry(const collada::MeshGeometry &mesh_geo)
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
    }* /

    / *void Geometry::loadFromCollada(const collada::MeshGeometry &mesh_geo) {
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
    }* /

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

    void Geometry::createArrayAndBuffers() {
        destroyArrayAndBuffers();
        commitNewVertex();

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

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

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        m_buffers_created = true;
    }

    void Geometry::destroyArrayAndBuffers() {
        if (m_buffers_created) {
            if (glIsBuffer(m_data_buffer)) {
                glDeleteBuffers(1, &m_data_buffer);
            }

            if (glIsBuffer(m_element_buffer)) {
                glDeleteBuffers(1, &m_element_buffer);
            }

            if (glIsVertexArray(m_vao)) {
                glDeleteVertexArrays(1, &m_vao);
            }
        }

        m_buffers_created = false;
        m_vao_initialized = false;
    }

    void Geometry::setUpVertexArray(const Material &material) {
        if (!m_buffers_created) {
            createArrayAndBuffers();
        }

        GLint pos_loc = material.getPositionLocation();
        GLint norm_loc = material.getNormalLocation();
        GLint color_loc = material.getColorLocation();
        GLint tc_loc = material.getTexCoordLocation();
        GLsizeiptr vertex_size = m_stride * sizeof(float);

        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_data_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_element_buffer);

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

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        m_vao_initialized = true;
    }

    void Geometry::render(const glm::mat4x4 &model, const Material &material) const {
        GLint model_loc = material.getModelLocation();
        GLint model_inv_trans_3_loc = material.getModelInverseTranspose3Location();

        glBindVertexArray(m_vao);

        if (model_loc >= 0) {
            glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
        }

        if (model_inv_trans_3_loc >= 0) {
            glm::mat3x3 model_inv_trans_3 = glm::inverseTranspose(glm::mat3x3(model));
            glUniformMatrix3fv(model_inv_trans_3_loc, 1, GL_FALSE, glm::value_ptr(model_inv_trans_3));
        }

        glDrawElements(m_draw_type, m_vertex_elems.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
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

    struct PCNVertex {
        float position[3];
        float color[4];
        float normal[3];
    };

    const PCNVertex OCTOHEDRON_VERTEX_ARRAY[24] = {
        // Position        Color           Normal
        {  {  0,  0,  1 }, { 0, 0, 1, 1 }, {  0.577,  0.577,  0.577 } },
        {  {  1,  0,  0 }, { 1, 0, 0, 1 }, {  0.577,  0.577,  0.577 } },
        {  {  0,  1,  0 }, { 0, 1, 0, 1 }, {  0.577,  0.577,  0.577 } },

        {  {  0,  0,  1 }, { 0, 0, 1, 1 }, { -0.577,  0.577,  0.577 } },
        {  {  0,  1,  0 }, { 0, 1, 0, 1 }, { -0.577,  0.577,  0.577 } },
        {  { -1,  0,  0 }, { 1, 0, 0, 1 }, { -0.577,  0.577,  0.577 } },

        {  {  0,  0,  1 }, { 0, 0, 1, 1 }, { -0.577, -0.577,  0.577 } },
        {  { -1,  0,  0 }, { 1, 0, 0, 1 }, { -0.577, -0.577,  0.577 } },
        {  {  0, -1,  0 }, { 0, 1, 0, 1 }, { -0.577, -0.577,  0.577 } },

        {  {  0,  0,  1 }, { 0, 0, 1, 1 }, {  0.577, -0.577,  0.577 } },
        {  {  0, -1,  0 }, { 0, 1, 0, 1 }, {  0.577, -0.577,  0.577 } },
        {  {  1,  0,  0 }, { 1, 0, 0, 1 }, {  0.577, -0.577,  0.577 } },

        {  {  0,  0, -1 }, { 0, 0, 1, 1 }, {  0.577,  0.577, -0.577 } },
        {  {  0,  1,  0 }, { 0, 1, 0, 1 }, {  0.577,  0.577, -0.577 } },
        {  {  1,  0,  0 }, { 1, 0, 0, 1 }, {  0.577,  0.577, -0.577 } },

        {  {  0,  0, -1 }, { 0, 0, 1, 1 }, { -0.577,  0.577, -0.577 } },
        {  { -1,  0,  0 }, { 1, 0, 0, 1 }, { -0.577,  0.577, -0.577 } },
        {  {  0,  1,  0 }, { 0, 1, 0, 1 }, { -0.577,  0.577, -0.577 } },

        {  {  0,  0, -1 }, { 0, 0, 1, 1 }, { -0.577, -0.577, -0.577 } },
        {  {  0, -1,  0 }, { 0, 1, 0, 1 }, { -0.577, -0.577, -0.577 } },
        {  { -1,  0,  0 }, { 1, 0, 0, 1 }, { -0.577, -0.577, -0.577 } },

        {  {  0,  0, -1 }, { 0, 0, 1, 1 }, {  0.577, -0.577, -0.577 } },
        {  {  1,  0,  0 }, { 1, 0, 0, 1 }, {  0.577, -0.577, -0.577 } },
        {  {  0, -1,  0 }, { 0, 1, 0, 1 }, {  0.577, -0.577, -0.577 } },
    };

    // This is just each of the vertex data in order, since each
    // repeated position / color has a different normal...
    const unsigned int OCTOHEDRON_VERTEX_ELEMS[24] = {
         0,  1,  2,
         3,  4,  5,
         6,  7,  8,
         9, 10, 11,
        12, 13, 14,
        15, 16, 17,
        18, 19, 20,
        21, 22, 23,
    };

    OctohedronGeometry::OctohedronGeometry() : Geometry() {
        m_position_offset = 0;
        m_normal_offset = 7;
        m_color_offset = 3;
        m_stride = 10;

        m_vertex_attrs.resize(24 * m_stride);
        m_vertex_elems.resize(24);
        for (unsigned int i = 0; i < 24; ++i) {
            m_vertex_elems[i] = OCTOHEDRON_VERTEX_ELEMS[i];

            m_vertex_attrs[m_stride*i]   = OCTOHEDRON_VERTEX_ARRAY[i].position[0];
            m_vertex_attrs[m_stride*i+1] = OCTOHEDRON_VERTEX_ARRAY[i].position[1];
            m_vertex_attrs[m_stride*i+2] = OCTOHEDRON_VERTEX_ARRAY[i].position[2];

            m_vertex_attrs[m_stride*i+3] = OCTOHEDRON_VERTEX_ARRAY[i].color[0];
            m_vertex_attrs[m_stride*i+4] = OCTOHEDRON_VERTEX_ARRAY[i].color[1];
            m_vertex_attrs[m_stride*i+5] = OCTOHEDRON_VERTEX_ARRAY[i].color[2];
            m_vertex_attrs[m_stride*i+6] = OCTOHEDRON_VERTEX_ARRAY[i].color[3];

            m_vertex_attrs[m_stride*i+7] = OCTOHEDRON_VERTEX_ARRAY[i].normal[0];
            m_vertex_attrs[m_stride*i+8] = OCTOHEDRON_VERTEX_ARRAY[i].normal[1];
            m_vertex_attrs[m_stride*i+9] = OCTOHEDRON_VERTEX_ARRAY[i].normal[2];
        }
    }

    / * CubeGeometry::CubeGeometry() : Geometry() {
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
    }* /

    struct PositionsAndElements {
        std::vector<glm::vec3> verts;
        std::vector<unsigned int> elems;
    };

    int index_of(std::vector<glm::vec3> &vec, const glm::vec3 &val) {
        for (unsigned int i = 0; i < vec.size(); ++i) {
            if (glm::all(glm::epsilonEqual(vec[i], val, glm::epsilon<float>()))) {
                return i;
            }
        }
        return -1;
    }

    unsigned int add_position_to_vec(std::vector<glm::vec3> &vec, glm::vec3 pos) {
        int index = index_of(vec, pos);
        if (index < 0) {
            index = vec.size();
            vec.emplace_back(pos);
        }
        return static_cast<unsigned int>(index);
    }

    PositionsAndElements refine(PositionsAndElements &old_verts) {
        PositionsAndElements new_verts;

        for (unsigned int i = 0; i < old_verts.elems.size(); i += 3) {
            glm::vec3
                &p1 = old_verts.verts[old_verts.elems[i]],
                &p2 = old_verts.verts[old_verts.elems[i+1]],
                &p3 = old_verts.verts[old_verts.elems[i+2]];

            unsigned int
                p1i = add_position_to_vec(new_verts.verts, p1),
                p2i = add_position_to_vec(new_verts.verts, p2),
                p3i = add_position_to_vec(new_verts.verts, p3),
                p4i = add_position_to_vec(new_verts.verts, glm::normalize((p1 + p2) * 0.5f)),
                p5i = add_position_to_vec(new_verts.verts, glm::normalize((p2 + p3) * 0.5f)),
                p6i = add_position_to_vec(new_verts.verts, glm::normalize((p1 + p3) * 0.5f));

            new_verts.elems.emplace_back(p1i);
            new_verts.elems.emplace_back(p4i);
            new_verts.elems.emplace_back(p6i);

            new_verts.elems.emplace_back(p4i);
            new_verts.elems.emplace_back(p2i);
            new_verts.elems.emplace_back(p5i);

            new_verts.elems.emplace_back(p6i);
            new_verts.elems.emplace_back(p5i);
            new_verts.elems.emplace_back(p3i);

            new_verts.elems.emplace_back(p6i);
            new_verts.elems.emplace_back(p4i);
            new_verts.elems.emplace_back(p5i);
        }

        return new_verts;
    }

    SphereGeometry::SphereGeometry() : Geometry() {
        PositionsAndElements pne, prev_pne;

        // Use the octohedron, but only the positions. This means that
        // we can avoid duplication of vertices with the element
        // array.
        for (unsigned int i = 0; i < 24; ++i) {
            glm::vec3 pos(
                OCTOHEDRON_VERTEX_ARRAY[i].position[0],
                OCTOHEDRON_VERTEX_ARRAY[i].position[1],
                OCTOHEDRON_VERTEX_ARRAY[i].position[2]);
            pne.elems.emplace_back(add_position_to_vec(pne.verts, std::move(pos)));
        }

        // Run the refinements. We don't need to do this a lot.
        for (unsigned int i = 0; i < 4; ++i) {
            std::swap(pne, prev_pne);
            pne = refine(prev_pne);
        }

        // Initialize this geometry.
        m_position_offset = 0;
        m_normal_offset = 3;
        m_color_offset = 6;
        m_stride = 10;
        m_vertex_attrs.resize(pne.verts.size() * m_stride);
        m_vertex_elems = pne.elems;

        for (unsigned int i = 0; i < pne.verts.size(); ++i) {
            glm::vec3 pos = glm::normalize(pne.verts[i]);

            // Position.
            m_vertex_attrs[i*m_stride] =   pos.x;
            m_vertex_attrs[i*m_stride+1] = pos.y;
            m_vertex_attrs[i*m_stride+2] = pos.z;

            // Normal. (same as position, since it's the unit sphere)
            m_vertex_attrs[i*m_stride+3] = pos.x;
            m_vertex_attrs[i*m_stride+4] = pos.y;
            m_vertex_attrs[i*m_stride+5] = pos.z;

            // Color.
            m_vertex_attrs[i*m_stride+6] = std::abs(pos.x);
            m_vertex_attrs[i*m_stride+7] = std::abs(pos.y);
            m_vertex_attrs[i*m_stride+8] = std::abs(pos.z);
            m_vertex_attrs[i*m_stride+9] = 1.0f;
        }
    }
}; */

#ifndef _GRAPHPLAY_GRAPHPLAY_GEOMETRY_CPP_
#define _GRAPHPLAY_GRAPHPLAY_GEOMETRY_CPP_

#include "graphplay.h"
#include "Geometry.h"

namespace graphplay {
    template <typename V>
    Geometry<V>::Geometry()
        : AbstractGeometry(),
          m_vertices(),
          m_elems(),
          m_attr_infos()
    {}

    template <typename V>
    Geometry<V>::~Geometry() {}

    template <typename V>
    AbstractGeometry& Geometry<V>::operator=(const AbstractGeometry &abstract_other) {
        const Geometry<V> &other = dynamic_cast<const Geometry<V>&>(abstract_other);
        m_elem_buffer = duplicateBuffer(GL_ELEMENT_ARRAY_BUFFER, other.m_elem_buffer);
        m_vertex_buffer = duplicateBuffer(GL_ARRAY_BUFFER, other.m_vertex_buffer);
        m_array_object = duplicateVertexArrayObject(other.m_array_object);
        m_vertices = other.m_vertices;
        m_elems = other.m_elems;
        m_attr_infos = other.m_attr_infos;
        return *this;
    }

    template <typename V>
    AbstractGeometry& Geometry<V>::operator=(AbstractGeometry &&abstract_other) {
        Geometry<V> &other = dynamic_cast<Geometry<V>&>(abstract_other);
        std::swap(m_vertex_buffer, other.m_vertex_buffer);
        std::swap(m_elem_buffer, other.m_elem_buffer);
        std::swap(m_array_object, other.m_array_object);
        std::swap(m_vertices, other.m_vertices);
        std::swap(m_elems, other.m_elems);
        std::swap(m_attr_infos, other.m_attr_infos);
        return *this;
    }

    template <typename V>
    void Geometry<V>::setVertexData(
        const typename Geometry<V>::elem_array_type &new_elems,
        const typename Geometry<V>::vertex_array_type &new_verts)
    {
        m_elems = new_elems;
        m_vertices = new_verts;
    }

    template <typename V>
    void Geometry<V>::setVertexData(
        typename Geometry<V>::elem_array_type &&new_elems,
        typename Geometry<V>::vertex_array_type &&new_verts)
    {
        m_elems = new_elems;
        m_vertices = new_verts;
    }

    template <typename V>
    void Geometry<V>::setVertexData(
        const typename Geometry<V>::elem_type *const elems, unsigned int num_elems,
        const typename Geometry<V>::vertex_type *const verts, unsigned int num_verts)
    {
        setVertexData(
            typename Geometry<V>::elem_array_type(elems, &elems[num_elems]),
            typename Geometry<V>::vertex_array_type(verts, &verts[num_verts]));
    }

    template <typename V>
    void Geometry<V>::createBuffers() {
        deleteBuffers();

        GLuint buffers[2];
        glGenBuffers(2, buffers);
        m_vertex_buffer = buffers[0];
        m_elem_buffer = buffers[1];

        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER,
            m_vertices.size()*sizeof(Geometry<V>::vertex_type),
            m_vertices.data(),
            GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elem_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            m_elems.size()*sizeof(Geometry<V>::elem_type),
            m_elems.data(),
            GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    template <typename V>
    void Geometry<V>::createVertexArray(const Shader &shader) {
        deleteVertexArray();
        if (!glIsBuffer(m_vertex_buffer) || !glIsBuffer(m_elem_buffer)) {
            createBuffers();
        }

        glGenVertexArrays(1, &m_array_object);
        glBindVertexArray(m_array_object);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elem_buffer);

        auto shader_attrs = shader.getAttributes();
        for (auto geo_attr : m_attr_infos) {
            auto shader_attr = shader_attrs.find(geo_attr.first);
            if (shader_attr != shader_attrs.end()) {
                glVertexAttribPointer(
                    shader_attr->second,
                    geo_attr.second.count,
                    geo_attr.second.type,
                    GL_FALSE,
                    sizeof(V),
                    BUFFER_OFFSET_BYTES(geo_attr.second.offset));
            }
        }

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    template <typename V>
    void Geometry<V>::render() const {
        glBindVertexArray(m_array_object);
        glDrawElements(GL_TRIANGLES, m_elems.size(), elem_gl_type, BUFFER_OFFSET_BYTES(0));
        glBindVertexArray(0);
    }
};

#endif
