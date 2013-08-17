// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_GEOMETRY_H_
#define _GRAPHPLAY_GRAPHPLAY_GEOMETRY_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <gtest/gtest_prod.h>
#include <memory>
#include <vector>

namespace graphplay {
    class Material;

    class Geometry
    {
    public:
        Geometry();

        // These control the data on the GPU.
        void generateBuffers();
        void destroyBuffers();

        // These work sort of like OpenGL's immediate-vertex functions, if you
        // want to generate the geometry directly. You'll need to call 
        // generateBuffers() after calling them.
        void vertex3f(float x, float y, float z);
        void normal3f(float x, float y, float z);
        void color4f(float r, float g, float b, float a);
        void texCoord2f(float s, float t);
        
        void clearVertices();

        void render(const glm::mat4x4 &projection, const glm::mat4x4 &model_view, const Material &material) const;

    protected:
        void commitNewVertex();
        unsigned int findVertex(std::vector<float> &vdata);

    private:
        std::vector<float> m_vertex_attrs;
        std::vector<GLuint> m_vertex_elems;
        int m_position_offset, m_normal_offset, m_color_offset, m_tex_coord_offset;
        unsigned int m_stride;

        std::vector<float> m_new_vertex;

        GLuint m_data_buffer;
        GLuint m_element_buffer;

        FRIEND_TEST(Geometry, DefaultConstructor);
    };

    typedef std::unique_ptr<Geometry> up_Geometry;
    typedef std::shared_ptr<Geometry> sp_Geometry;
    typedef std::weak_ptr<Geometry> wp_Geometry;

    class OctohedronGeometry : public Geometry
    {
    public:
        OctohedronGeometry();
    };
};

#endif
