// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "graphplay.h"

#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <sstream>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/range.hpp>
#include <glm/gtx/io.hpp>

#include "OpenGLUtils.h"
#include "Geometry.h"
#include "PlyFile.h"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

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

    std::ostream& operator<<(std::ostream& stream, const PCNVertex &vertex) {
        std::stringstream buf;
        buf << "{ position = ["
            << vertex.position[0] << ", "
            << vertex.position[1] << ", "
            << vertex.position[2]
            << "], color = ["
            << vertex.color[0] << ", "
            << vertex.color[1] << ", "
            << vertex.color[2] << ", "
            << vertex.color[3]
            << "], normal = ["
            << vertex.normal[0] << ", "
            << vertex.normal[1] << ", "
            << vertex.normal[2]
            << "] }";
        return stream << buf.str();
    }

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
        Geometry<PCNVertex>::sptr_type rv = std::make_shared<Geometry<PCNVertex> >();
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
            index = (int)vec.size();
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
        Geometry<PCNVertex>::sptr_type rv = std::make_shared<Geometry<PCNVertex> >();
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

    Geometry<PCNVertex>::sptr_type loadPCNFile(const char *filename) {
        Geometry<PCNVertex>::sptr_type rv = std::make_shared<Geometry<PCNVertex> >();
        std::fstream file(filename, std::ios::in | std::ios::binary);
        unsigned int num_things = 0;
        char magic_arr[4];
        std::string magic;

        file.read(magic_arr, 4);
        magic = magic_arr;
        if (magic == "pcn") {
            file.read(reinterpret_cast<char *>(&num_things), 4);
            std::cout << "Reading " << num_things << " vertices from " << filename << std::endl;
            Geometry<PCNVertex>::vertex_array_type verts(num_things);
            file.read(reinterpret_cast<char *>(verts.data()), num_things*sizeof(PCNVertex));

            file.read(reinterpret_cast<char *>(&num_things), 4);
            std::cout << "Reading " << num_things << " elements from " << filename << std::endl;
            Geometry<PCNVertex>::elem_array_type elems(num_things);
            file.read(reinterpret_cast<char *>(elems.data()), num_things*sizeof(unsigned int));

            rv->setVertexData(std::move(elems), std::move(verts));
            rv->createBuffers();
        } else {
            std::cerr << "File " << filename << " does not appear to be a PCN file." << std::endl;
        }
        
        file.close();
        return rv;
    }

    Geometry<PCNVertex>::sptr_type loadPlyFile(const char *filename) {
        Geometry<PCNVertex>::sptr_type rv = std::make_shared<Geometry<PCNVertex> >();
        Geometry<PCNVertex>::vertex_array_type verts;
        Geometry<PCNVertex>::elem_array_type elems;
        std::fstream file(filename, std::ios::in | std::ios::binary);
        PlyFile f(file);
        file.close();

        // Read the vertex array data.
        const Element *vertex_elem = f.getElement("vertex");
        if (vertex_elem != nullptr) {
            const std::vector<Property> vertex_props = vertex_elem->properties();
            const std::vector<ElementValue> &ply_vertex_data = vertex_elem->data();
            verts.resize(vertex_elem->count());

            for (auto &&prop : vertex_props) {
                const std::string &pname = prop.name();
                unsigned int i = 0;
                unsigned int offset = 0;

                if (pname == "x") {
                    offset = offsetof(PCNVertex, position[0]);
                } else if (pname == "y") {
                    offset = offsetof(PCNVertex, position[1]);
                } else if (pname == "z") {
                    offset = offsetof(PCNVertex, position[2]);
                } else if (pname == "red") {
                    offset = offsetof(PCNVertex, color[0]);
                } else if (pname == "green") {
                    offset = offsetof(PCNVertex, color[1]);
                } else if (pname == "blue") {
                    offset = offsetof(PCNVertex, color[2]);
                } else if (pname == "alpha") {
                    offset = offsetof(PCNVertex, color[3]);
                } else if (pname == "nx") {
                    offset = offsetof(PCNVertex, normal[0]);
                } else if (pname == "ny") {
                    offset = offsetof(PCNVertex, normal[1]);
                } else if (pname == "nz") {
                    offset = offsetof(PCNVertex, normal[2]);
                } else {
                    continue;
                }

                for (auto &&elem_val : ply_vertex_data) {
                    const PropertyValue &prop_val = elem_val.getProperty(prop.name());
                    float *dst = reinterpret_cast<float*>(reinterpret_cast<char*>(&verts[i]) + offset);
                    *dst = prop_val.first<float>();
                    ++i;
                }
            }
        } else {
            std::cerr << "File " << filename << " did not have a \"vertex\" element." << std::endl;
        }

        // Read the element array data.
        const Element *faces_elem = f.getElement("face");
        if (faces_elem != nullptr) {
            const std::vector<Property> face_props = faces_elem->properties();
            const std::vector<ElementValue> &ply_elem_data = faces_elem->data();

            for (auto&& prop : face_props) {
                if (prop.name() == "vertex_indices") {
                    for (auto &&elem_val : ply_elem_data) {
                        const PropertyValue &prop_val = elem_val.getProperty(prop.name());
                        for (PropertyValueIterator<Geometry<PCNVertex>::elem_type> index = prop_val.begin<Geometry<PCNVertex>::elem_type>();
                             index != prop_val.end<Geometry<PCNVertex>::elem_type>();
                             ++index) {
                            elems.emplace_back(*index);
                        }
                    }
                }
            }
        } else {
            std::cerr << "File " << filename << " did not have a \"face\" element." << std::endl;
        }

        // Determine the bounding box of the mesh.
        glm::vec3 bb_min, bb_max;
        for (Geometry<PCNVertex>::vertex_array_type::iterator v = verts.begin(); v != verts.end(); ++v) {
            glm::vec3 pos = glm::make_vec3(v->position);

            if (v == verts.begin()) {
                bb_min = bb_max = pos;
            }

            bb_min = glm::min(bb_min, pos);
            bb_max = glm::max(bb_max, pos);
        }

        // Convert all the positions to be between -1 and 1 with the
        // barycenter at the origin.
        glm::vec3 bcenter = (bb_min + bb_max) / 2.0f;
        glm::vec3 new_bb_max = bb_max - bcenter;
        float max_dim = *std::max_element(glm::begin(new_bb_max), glm::end(new_bb_max));
        for (Geometry<PCNVertex>::vertex_array_type::iterator v = verts.begin(); v != verts.end(); ++v) {
            glm::vec3 pos = glm::make_vec3(v->position);
            pos = (pos - bcenter) / max_dim;
            v->position[0] = pos.x;
            v->position[1] = pos.y;
            v->position[2] = pos.z;
        }

        // Compute the colors assuming each vertex is opaque.
        for (Geometry<PCNVertex>::vertex_array_type::iterator v = verts.begin(); v != verts.end(); ++v) {
            v->color[0] = (v->color[0] / v->color[3]) * std::abs(v->position[0]);
            v->color[1] = (v->color[1] / v->color[3]) * std::abs(v->position[1]);
            v->color[2] = (v->color[2] / v->color[3]) * std::abs(v->position[2]);
            v->color[3] = 1.0;
        }

        rv->setVertexData(std::move(elems), std::move(verts));
        return rv;
    }
}
