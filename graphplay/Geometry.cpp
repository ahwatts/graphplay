// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "Geometry.h"

namespace graphplay {
    GLuint duplicateBuffer(GLenum target, GLuint src);
    GLuint duplicateVertexArrayObject(GLuint src);

    AbstractGeometry::AbstractGeometry()
        : m_array_object{ 0 },
        m_elem_buffer{ 0 },
        m_vertex_buffer{ 0 }
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

    Geometry<PNCVertex>* makeOctohedronGeometry() {
        return nullptr;
    }

    Geometry<PNCVertex>* makeSphereGeometry() {
        return nullptr;
    }

    GLuint duplicateBuffer(GLenum target, GLuint src) {
    }

    GLuint duplicateVertexArrayObject(GLuint src) {
    }
}
