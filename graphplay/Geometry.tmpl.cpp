// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

/* CubeGeometry::CubeGeometry() : Geometry() {
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
};*/

#ifndef _GRAPHPLAY_GRAPHPLAY_GEOMETRY_CPP_
#define _GRAPHPLAY_GRAPHPLAY_GEOMETRY_CPP_

#include "Geometry.h"
#include "Shader.h"
#include <iostream>

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

        glGenVertexArrays(1, &m_array_object);
        glBindVertexArray(m_array_object);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elem_buffer);

        auto shader_attrs = program.getAttributes();
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
