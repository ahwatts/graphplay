// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_GEOMETRY_H_
#define _GRAPHPLAY_GRAPHPLAY_GEOMETRY_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <map>
#include <string>
#include <vector>

// Do a forward declaration of MeshGeometry to reduce header
// pollution.
namespace collada {
    class MeshGeometry;
};

namespace graphplay {
    class Geometry
    {
    public:
        Geometry();
        Geometry(const collada::MeshGeometry &mesh_geo);

        void generateBuffers();

    private:
        std::vector<float> m_vertex_attrs;
        std::vector<GLuint> m_vertex_elems;
        unsigned int m_position_offset, m_normal_offset, m_color_offset, m_tex_coord_offset;
        unsigned int m_stride;

        GLuint m_data_buffer;
        GLuint m_element_buffer;
    };
};

#endif
