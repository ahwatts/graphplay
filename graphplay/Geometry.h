// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_GEOMETRY_H_
#define _GRAPHPLAY_GRAPHPLAY_GEOMETRY_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
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

    private:
        /* std::vector<glm::vec3> m_positions;
        std::vector<glm::vec3> m_normals;
        std::vector<glm::vec4> m_colors;
        std::vector<glm::vec2> m_tex_coords;
        std::vector<GLuint> m_indices;*/

        std::vector<float> m_vertex_attrs;

        GLuint m_data_buffer;
        GLuint m_element_buffer;
    };
};

#endif
