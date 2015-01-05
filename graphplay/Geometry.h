// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_GEOMETRY_H_
#define _GRAPHPLAY_GRAPHPLAY_GEOMETRY_H_

#include <memory>
#include <vector>
#include <glm/mat4x4.hpp>

#include "opengl.h"

namespace graphplay {
    class Material;

    class Geometry
    {
    public:
        // Iterator mumbo-jumbo so that we can walk the vertex list.
        class VertexIterator;
        friend class VertexIterator;
        typedef VertexIterator iterator;
        typedef std::ptrdiff_t difference_type;
        typedef std::size_t size_type;
        typedef std::vector<float> value_type;
        typedef value_type* pointer;
        typedef value_type& reference;

        Geometry();
        ~Geometry();

        // Iterator functions.
        iterator begin() const;
        iterator end() const;
        value_type get(unsigned int index) const;

        // These control the data on the GPU.
        void createArrayAndBuffers();
        void destroyArrayAndBuffers();
        void setUpVertexArray(const Material &material);

        inline GLuint getVertexArrayObject() const { return m_vao; }
        inline GLuint getArrayBuffer() const { return m_data_buffer; }
        inline GLuint getElementArrayBuffer() const { return m_element_buffer; }
        inline GLenum getDrawType() const { return m_draw_type; }

        // Renders the geometry.
        void render(const glm::mat4x4 &projection, const glm::mat4x4 &model_view, const Material &material) const;

        // These work sort of like OpenGL's immediate-vertex functions, if you
        // want to generate the geometry directly. You'll need to call 
        // generateBuffers() after calling them.
        void vertex3f(float x, float y, float z);
        void normal3f(float x, float y, float z);
        void color4f(float r, float g, float b, float a);
        void texCoord2f(float s, float t);
        void commitNewVertex();
        void clearVertices();

        // Getters for the various data parameters.
        inline unsigned int getNumVertices() const { return m_vertex_elems.size(); }
        inline unsigned int getStride() const { return m_stride; }
        inline int getPositionOffset() const { return m_position_offset; }
        inline int getNormalOffset() const { return m_normal_offset; }
        inline int getColorOffset() const { return m_color_offset; }
        inline int getTexCoordOffset() const { return m_tex_coord_offset; }
        
    protected:
        unsigned int findVertex(std::vector<float> &vdata);
        GLenum m_draw_type;

        std::vector<float> m_vertex_attrs;
        std::vector<GLuint> m_vertex_elems;
        int m_position_offset, m_normal_offset, m_color_offset, m_tex_coord_offset;
        unsigned int m_stride;

        std::vector<float> m_new_vertex;
        bool m_new_vertex_started;

        GLuint m_vao;
        GLuint m_data_buffer;
        GLuint m_element_buffer;
        bool m_buffers_created;
        bool m_vao_initialized;

    public:
        class VertexIterator {
        public:
            VertexIterator(const Geometry &geo, unsigned int init_loc);

            bool operator==(const VertexIterator &other) const;
            bool operator!=(const VertexIterator &other) const;
            Geometry::value_type operator*();
            VertexIterator &operator++();   // prefix
            VertexIterator operator++(int); // postfix

        private:
            VertexIterator();

            const Geometry &m_geo;
            unsigned int m_loc;
        };
    };

    typedef std::unique_ptr<Geometry> up_Geometry;
    typedef std::shared_ptr<Geometry> sp_Geometry;
    typedef std::weak_ptr<Geometry> wp_Geometry;

    // Subclasses with specific kinds of geometries.
    class OctohedronGeometry : public Geometry { public: OctohedronGeometry(); };
    class CubeGeometry : public Geometry { public: CubeGeometry(); };
    class NormalGeometry : public Geometry { public: NormalGeometry(const Geometry& base); };
};

#endif
