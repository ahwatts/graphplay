// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <algorithm>

#include "graphplay.h"
#include "OpenGLUtils.h"
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

    AbstractGeometry& AbstractGeometry::operator=(const AbstractGeometry &other) {
        AbstractGeometry tmp(other);
        std::swap(*this, tmp);
        return *this;
    }

    AbstractGeometry& AbstractGeometry::operator=(AbstractGeometry &&other) {
        std::swap(m_array_object, other.m_array_object);
        std::swap(m_elem_buffer, other.m_elem_buffer);
        std::swap(m_vertex_buffer, other.m_vertex_buffer);
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

    void AbstractGeometry::createVertexArray(const Program &program) {}

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
        { "normal",   VertexDesc { BUFFER_OFFSET_BYTES(7*sizeof(float)), GL_FLOAT, 3 } },
    };

    // Octohedron geometry builder.
#ifdef MSVC
    const
#else
    constexpr
#endif 
    PCNVertex OCTOHEDRON_VERTEX_ARRAY[24] = {
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
#ifdef MSVC
    const
#else
    constexpr
#endif
    unsigned int OCTOHEDRON_VERTEX_ELEMS[24] = {
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
}
