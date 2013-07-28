#ifndef _GRAPHPLAY_GRAPHPLAY_GEOMETRY_H_
#define _GRAPHPLAY_GRAPHPLAY_GEOMETRY_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "Material.h"

namespace graphplay {
    class Geometry
    {
    public:
        Geometry();
        virtual ~Geometry();

        void generateBuffers();
        void render(Material &material);

    private:
        std::vector<glm::vec3> m_positions;
        std::vector<glm::vec3> m_normals;
        std::vector<glm::vec4> m_colors;
        std::vector<GLuint> m_indices;

        GLuint m_data_buffer;
        GLuint m_element_buffer;
    };
};

#endif
