// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_GEOMETRY_CPP_
#define _GRAPHPLAY_GRAPHPLAY_GEOMETRY_CPP_

#include "OpenGLUtils.h"
#include "Shader.h"

namespace graphplay {
    template <typename V>
    Geometry<V>::Geometry()
        : AbstractGeometry(),
          m_vertices(),
          m_elems(),
          m_attr_infos(V::description)
    {
        // std::cout << "Geometry<V> default constructor: " << this << std::endl;
    }

    template <typename V>
    Geometry<V>::Geometry(const Geometry<V> &other)
        : AbstractGeometry(dynamic_cast<const AbstractGeometry&>(other)),
          m_vertices(other.m_vertices),
          m_elems(other.m_elems),
          m_attr_infos(V::description)
    {
        // std::cout << "Geometry<V> copy constructor: " << &other << " -> " << this << std::endl;
    }

    template <typename V>
    Geometry<V>::Geometry(Geometry<V> &&other)
        : m_vertices(std::move(other.m_vertices)),
          m_elems(std::move(other.m_elems)),
          m_attr_infos(V::description)
    {
        m_vertex_buffer = other.m_vertex_buffer;
        m_elem_buffer = other.m_elem_buffer;
        m_array_object = other.m_array_object;
        other.m_vertex_buffer = 0;
        other.m_elem_buffer = 0;
        other.m_array_object = 0;

        // std::cout << "Geometry<V> move constructor: " << &other << " -> " << this << std::endl;
    }

    template <typename V>
    Geometry<V>::~Geometry() {}

    template <typename V>
    AbstractGeometry& Geometry<V>::operator=(const AbstractGeometry &abstract_other) {
        // std::cout << "Geometry<V> abstract geometry copy assignment: " << &abstract_other << " -> " << this << std::endl;
        const Geometry<V> &other = dynamic_cast<const Geometry<V>&>(abstract_other);
        return *this = other;
    }

    template <typename V>
    Geometry<V>& Geometry<V>::operator=(const Geometry<V> &other) {
        // std::cout << "Geometry<V> geometry copy assignment: " << &other << " -> " << this << std::endl;
        Geometry<V> tmp(other);
        std::swap(*this, tmp);
        return *this;
    }

    template <typename V>
    AbstractGeometry& Geometry<V>::operator=(AbstractGeometry &&abstract_other) {
        // std::cout << "Geometry<V> abstract geometry move assignment: " << &abstract_other << " -> " << this << std::endl;
        Geometry<V> &other = dynamic_cast<Geometry<V>&>(abstract_other);
        *this = std::move(other);
        return *this;
    }

    template <typename V>
    Geometry<V>& Geometry<V>::operator=(Geometry<V> &&other) {
        // std::cout << "Geometry<V> geometry move assignment: " << &other << " -> " << this << std::endl;
        std::swap(m_vertex_buffer, other.m_vertex_buffer);
        std::swap(m_elem_buffer, other.m_elem_buffer);
        std::swap(m_array_object, other.m_array_object);
        std::swap(m_vertices, other.m_vertices);
        std::swap(m_elems, other.m_elems);
        return *this;
    }

    template <typename V>
    void Geometry<V>::setVertexData(
        const typename Geometry<V>::elem_array_type &new_elems,
        const typename Geometry<V>::vertex_array_type &new_verts)
    {
        // std::cout << "Geometry<V> setVertexData copy from refs" << std::endl;
        m_elems = new_elems;
        m_vertices = new_verts;
    }

    template <typename V>
    void Geometry<V>::setVertexData(
        typename Geometry<V>::elem_array_type &&new_elems,
        typename Geometry<V>::vertex_array_type &&new_verts)
    {
        // std::cout << "Geometry<V> setVertexData move from refs" << std::endl;
        m_elems = std::move(new_elems);
        m_vertices = std::move(new_verts);
    }

    template <typename V>
    void Geometry<V>::setVertexData(
        const typename Geometry<V>::elem_type *const elems, unsigned int num_elems,
        const typename Geometry<V>::vertex_type *const verts, unsigned int num_verts)
    {
        // std::cout << "Geometry<V> setVertexData copy from pointers" << std::endl;
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
    void Geometry<V>::createVertexArray(const Program &program) {
        deleteVertexArray();
        if (!glIsBuffer(m_vertex_buffer) || !glIsBuffer(m_elem_buffer)) {
            createBuffers();
        }

        glUseProgram(program.getProgramId());
        glGenVertexArrays(1, &m_array_object);
        glBindVertexArray(m_array_object);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elem_buffer);

        auto shader_attrs = program.getAttributes();
        for (auto geo_attr : m_attr_infos) {
            auto shader_attr = shader_attrs.find(geo_attr.first);
            if (shader_attr != shader_attrs.end()) {
                glEnableVertexAttribArray(shader_attr->second);
                glVertexAttribPointer(
                    shader_attr->second,
                    geo_attr.second.count,
                    geo_attr.second.type,
                    GL_FALSE,
                    sizeof(Geometry<V>::vertex_type),
                    BUFFER_OFFSET_BYTES(geo_attr.second.offset));
            }
        }

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glUseProgram(0);
    }

    template <typename V>
    void Geometry<V>::render() const {
        glBindVertexArray(m_array_object);

        // static int i = 0;
        // if (i % 500 == 0) {
        //     dumpOpenGLState();
        // }
        // ++i;

        glDrawElements(GL_TRIANGLES, (GLsizei)m_elems.size(), elem_gl_type, BUFFER_OFFSET_BYTES(0));
        glBindVertexArray(0);
    }
};

#endif
