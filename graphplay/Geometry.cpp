// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <algorithm>

#include "opengl.h"

#include "Geometry.h"

namespace graphplay {
    // Class AbstractGeometry.
    AbstractGeometry::AbstractGeometry()
        : m_vertex_buffer{0},
          m_elem_buffer{0},
          m_array_object{0}
    {}

    AbstractGeometry::AbstractGeometry(const AbstractGeometry &other) : AbstractGeometry() {
        m_elem_buffer = duplicateBuffer(GL_ELEMENT_ARRAY_BUFFER, other.m_elem_buffer);
        m_vertex_buffer = duplicateBuffer(GL_ARRAY_BUFFER, other.m_vertex_buffer);
        m_array_object = duplicateVertexArrayObject(other.m_array_object);
    }

    AbstractGeometry::AbstractGeometry(AbstractGeometry &&other) : AbstractGeometry() {
        // Move other's GL objects over here.
        m_array_object = other.m_array_object;
        m_elem_buffer = other.m_elem_buffer;
        m_vertex_buffer = other.m_vertex_buffer;

        // Make other stop referencing its GL objects.
        other.m_array_object = 0;
        other.m_elem_buffer = 0;
        other.m_vertex_buffer = 0;
    }

    AbstractGeometry::~AbstractGeometry() {
        deleteBuffers();
        deleteVertexArray();
    }

    AbstractGeometry& AbstractGeometry::operator=(AbstractGeometry other) {
        std::swap(*this, other);
        return *this;
    }

    void AbstractGeometry::createBuffers() {}

    void AbstractGeometry::deleteBuffers() {
        if (glIsBuffer(m_elem_buffer)) {
            glDeleteBuffers(1, &m_elem_buffer);
        }

        if (glIsBuffer(m_vertex_buffer)) {
            glDeleteBuffers(1, &m_vertex_buffer);
        }

        m_vertex_buffer = 0;
        m_elem_buffer = 0;
    }

    void AbstractGeometry::createVertexArray(const Shader &shader) {}

    void AbstractGeometry::deleteVertexArray() {
        if (glIsVertexArray(m_array_object)) {
            glDeleteVertexArrays(1, &m_array_object);
        }

        m_array_object = 0;
    }

    void AbstractGeometry::render() const {}

    // Static PCNVertex description.
    const AttrMap PCNVertex::description {
        { "position", VertexDesc { BUFFER_OFFSET_BYTES(0*sizeof(float)), GL_FLOAT, 3 } },
        { "color",    VertexDesc { BUFFER_OFFSET_BYTES(3*sizeof(float)), GL_FLOAT, 4 } },
        { "normal",   VertexDesc { BUFFER_OFFSET_BYTES(7*sizeof(float)), GL_FLOAT, 3 } }
    };

    // Octohedron geometry builder.
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

    Geometry<PCNVertex>::sptr_type makeOctohedronGeometry() {
        Geometry<PCNVertex>::sptr_type rv = std::make_shared<Geometry<PCNVertex>>();
        rv->setVertexData(
            OCTOHEDRON_VERTEX_ELEMS, 24,
            OCTOHEDRON_VERTEX_ARRAY, 24);
        return rv;
    }

    // Geometry<PNCVertex>* makeSphereGeometry() {
    // }

    GLuint duplicateBuffer(GLenum target, GLuint src) {
        if (glIsBuffer(src)) {
            // Retrieve the size, usage, and data of the buffer.
            GLint size = 0, usage = 0;
            glBindBuffer(target, src);
            glGetBufferParameteriv(target, GL_BUFFER_SIZE, &size);
            glGetBufferParameteriv(target, GL_BUFFER_USAGE, &usage);
            char *src_data = (char *)glMapBuffer(target, GL_READ_ONLY);
            char *inter_data = new char[size];
            std::copy(src_data, src_data + size, inter_data);
            glUnmapBuffer(target);
            glBindBuffer(target, 0);

            // Copy the data to a new buffer.
            GLuint dst = 0;
            glGenBuffers(1, &dst);
            glBindBuffer(target, dst);
            glBufferData(target, size, inter_data, usage);
            glBindBuffer(target, 0);

            // Cleanup.
            delete [] inter_data;

            return dst;
        } else {
            return 0;
        }
    }

    struct VAPState {
        GLuint enabled;
        GLuint array_buffer_binding;
        GLuint size;
        GLuint stride;
        GLenum type;
        GLuint is_normalized;
        GLuint is_integer;
        GLuint divisor;
        GLvoid *offset;
    };

    GLuint duplicateVertexArrayObject(GLuint src) {
        if (glIsVertexArray(src)) {
            glBindVertexArray(src);

            GLint elem_binding = 0, max_attribs = 0;
            glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elem_binding);
            glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);

            VAPState *attribs = new VAPState[max_attribs];
            for (auto i = 0; i < max_attribs; ++i) {
                glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &attribs[i].enabled);
                if (attribs[i].enabled) {
                    glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &attribs[i].array_buffer_binding);
                    glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_SIZE, &attribs[i].size);
                    glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &attribs[i].stride);
                    glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_TYPE, &attribs[i].type);
                    glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &attribs[i].is_normalized);
                    glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_INTEGER, &attribs[i].is_integer);
                    glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_DIVISOR, &attribs[i].divisor);
                    glGetVertexAttribPointerv(i, GL_VERTEX_ATTRIB_ARRAY_POINTER, &attribs[i].offset);
                }
            }

            glBindVertexArray(0);

            GLuint dst = 0;
            glGenVertexArrays(1, &dst);
            glBindVertexArray(dst);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elem_binding);
            for (auto i = 0; i < max_attribs; ++i) {
                if (attribs[i].enabled) {
                    glEnableVertexAttribArray(i);
                    glBindBuffer(GL_ARRAY_BUFFER, attribs[i].array_buffer_binding);
                    glVertexAttribDivisor(i, attribs[i].divisor);

                    switch (attribs[i].type) {
                    case GL_BYTE:
                    case GL_UNSIGNED_BYTE:
                    case GL_SHORT:
                    case GL_UNSIGNED_SHORT:
                    case GL_INT:
                    case GL_UNSIGNED_INT:
                        if (attribs[i].is_integer) {
                            glVertexAttribIPointer(i,
                                attribs[i].size,
                                attribs[i].type,
                                attribs[i].stride,
                                attribs[i].offset);
                        } else {
                            glVertexAttribPointer(i,
                                attribs[i].size,
                                attribs[i].type,
                                attribs[i].is_normalized,
                                attribs[i].stride,
                                attribs[i].offset);
                        }
                        break;
                    case GL_DOUBLE:
                        glVertexAttribLPointer(i,
                            attribs[i].size,
                            attribs[i].type,
                            attribs[i].stride,
                            attribs[i].offset);
                        break;
                    default:
                        glVertexAttribPointer(i,
                            attribs[i].size,
                            attribs[i].type,
                            attribs[i].is_normalized,
                            attribs[i].stride,
                            attribs[i].offset);
                        break;
                    }
                }
            }

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            delete [] attribs;
            return dst;
        } else {
            return 0;
        }
    }
}
