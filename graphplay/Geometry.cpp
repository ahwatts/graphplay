// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "graphplay.h"

#include <algorithm>
#include <memory>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtc/epsilon.hpp>

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
        //   Position                 Color                         Normal
        { {  0.0f,  0.0f,  1.0f, }, { 0.0f, 0.0f, 1.0f, 1.0f, }, { -0.577f,  0.577f,  0.577f, } },
        { {  0.0f,  1.0f,  0.0f, }, { 0.0f, 1.0f, 0.0f, 1.0f, }, { -0.577f,  0.577f,  0.577f, } },
        { { -1.0f,  0.0f,  0.0f, }, { 1.0f, 0.0f, 0.0f, 1.0f, }, { -0.577f,  0.577f,  0.577f, } },

        { {  0.0f,  0.0f,  1.0f, }, { 0.0f, 0.0f, 1.0f, 1.0f, }, {  0.577f,  0.577f,  0.577f, } },
        { {  1.0f,  0.0f,  0.0f, }, { 1.0f, 0.0f, 0.0f, 1.0f, }, {  0.577f,  0.577f,  0.577f, } },
        { {  0.0f,  1.0f,  0.0f, }, { 0.0f, 1.0f, 0.0f, 1.0f, }, {  0.577f,  0.577f,  0.577f, } },

        { {  0.0f,  0.0f,  1.0f, }, { 0.0f, 0.0f, 1.0f, 1.0f, }, { -0.577f, -0.577f,  0.577f, } },
        { { -1.0f,  0.0f,  0.0f, }, { 1.0f, 0.0f, 0.0f, 1.0f, }, { -0.577f, -0.577f,  0.577f, } },
        { {  0.0f, -1.0f,  0.0f, }, { 0.0f, 1.0f, 0.0f, 1.0f, }, { -0.577f, -0.577f,  0.577f, } },

        { {  0.0f,  0.0f,  1.0f, }, { 0.0f, 0.0f, 1.0f, 1.0f, }, {  0.577f, -0.577f,  0.577f, } },
        { {  0.0f, -1.0f,  0.0f, }, { 0.0f, 1.0f, 0.0f, 1.0f, }, {  0.577f, -0.577f,  0.577f, } },
        { {  1.0f,  0.0f,  0.0f, }, { 1.0f, 0.0f, 0.0f, 1.0f, }, {  0.577f, -0.577f,  0.577f, } },

        { {  0.0f,  0.0f, -1.0f, }, { 0.0f, 0.0f, 1.0f, 1.0f, }, {  0.577f,  0.577f, -0.577f, } },
        { {  0.0f,  1.0f,  0.0f, }, { 0.0f, 1.0f, 0.0f, 1.0f, }, {  0.577f,  0.577f, -0.577f, } },
        { {  1.0f,  0.0f,  0.0f, }, { 1.0f, 0.0f, 0.0f, 1.0f, }, {  0.577f,  0.577f, -0.577f, } },

        { {  0.0f,  0.0f, -1.0f, }, { 0.0f, 0.0f, 1.0f, 1.0f, }, { -0.577f,  0.577f, -0.577f, } },
        { { -1.0f,  0.0f,  0.0f, }, { 1.0f, 0.0f, 0.0f, 1.0f, }, { -0.577f,  0.577f, -0.577f, } },
        { {  0.0f,  1.0f,  0.0f, }, { 0.0f, 1.0f, 0.0f, 1.0f, }, { -0.577f,  0.577f, -0.577f, } },

        { {  0.0f,  0.0f, -1.0f, }, { 0.0f, 0.0f, 1.0f, 1.0f, }, { -0.577f, -0.577f, -0.577f, } },
        { {  0.0f, -1.0f,  0.0f, }, { 0.0f, 1.0f, 0.0f, 1.0f, }, { -0.577f, -0.577f, -0.577f, } },
        { { -1.0f,  0.0f,  0.0f, }, { 1.0f, 0.0f, 0.0f, 1.0f, }, { -0.577f, -0.577f, -0.577f, } },

        { {  0.0f,  0.0f, -1.0f, }, { 0.0f, 0.0f, 1.0f, 1.0f, }, {  0.577f, -0.577f, -0.577f, } },
        { {  1.0f,  0.0f,  0.0f, }, { 1.0f, 0.0f, 0.0f, 1.0f, }, {  0.577f, -0.577f, -0.577f, } },
        { {  0.0f, -1.0f,  0.0f, }, { 0.0f, 1.0f, 0.0f, 1.0f, }, {  0.577f, -0.577f, -0.577f, } },
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
        rv->createBuffers();
        return rv;
    }

    // Creating a sphere by refinement of an octohedron.

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
                &p2 = old_verts.verts[old_verts.elems[i + 1]],
                &p3 = old_verts.verts[old_verts.elems[i + 2]];

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

    Geometry<PCNVertex>::sptr_type makeSphereGeometry() {
        Geometry<PCNVertex>::sptr_type rv = std::make_shared<Geometry<PCNVertex>>();
        Geometry<PCNVertex>::vertex_array_type verts;
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

        for (unsigned int i = 0; i < pne.verts.size(); ++i) {
            glm::vec3 pos = glm::normalize(pne.verts[i]);
            verts.emplace_back(
                PCNVertex {
                    { pos.x, pos.y, pos.z, },
                    { std::abs(pos.r), std::abs(pos.g), std::abs(pos.b), 1.0f, },
                    { pos.x, pos.y, pos.z, },
                });
        }

        rv->setVertexData(std::move(pne.elems), std::move(verts));
        rv->createBuffers();
        return rv;
    }
}
