// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_GEOMETRY_H_
#define _GRAPHPLAY_GRAPHPLAY_GEOMETRY_H_

/* #include <map>
#include <memory>
#include <vector>
#include <string>
#include <glm/mat4x4.hpp>

#include "opengl.h"

#include "Shader.h"

namespace graphplay {
    class Material;

    class Geometry
    {
    public:
        Geometry();
        ~Geometry();

        // These control the data on the GPU.
        void createArrayAndBuffers();
        void destroyArrayAndBuffers();
        void setUpVertexArray(const Material &material);

        inline GLuint getVertexArrayObject() const { return m_vao; }
        inline GLuint getArrayBuffer() const { return m_data_buffer; }
        inline GLuint getElementArrayBuffer() const { return m_element_buffer; }

        // Renders the geometry.
        void render(const glm::mat4x4 &model, const Material &material) const;

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
        // Iterator mumbo-jumbo so that we can walk the vertex list.
        class VertexIterator;

        friend class VertexIterator;
        typedef VertexIterator iterator;
        typedef std::ptrdiff_t difference_type;
        typedef std::size_t size_type;
        typedef std::vector<float> value_type;
        typedef value_type* pointer;
        typedef value_type& reference;

        iterator begin() const;
        iterator end() const;

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
    class SphereGeometry : public Geometry { public: SphereGeometry(); };
}; */

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "opengl.h"

#include "Shader.h"

namespace graphplay {
    struct VertexDesc
    {
        void *offset;
        GLenum type;
        unsigned int count;
    };

    class AbstractGeometry
    {
    public:
        typedef std::unique_ptr<AbstractGeometry> uptr_type;
        typedef std::shared_ptr<AbstractGeometry> sptr_type;
        typedef std::weak_ptr<AbstractGeometry> wptr_type;

        AbstractGeometry();
        AbstractGeometry(const AbstractGeometry &other);
        AbstractGeometry(AbstractGeometry &&other);
        virtual ~AbstractGeometry();

        virtual AbstractGeometry& operator=(AbstractGeometry other);

        virtual void createBuffers();
        virtual void deleteBuffers();
        virtual void createVertexArray(const Shader &shader);
        virtual void deleteVertexArray();

        virtual void render() const;

    protected:
        GLuint m_vertex_buffer;
        GLuint m_elem_buffer;
        GLuint m_array_object;
    };

    template <typename V>
    class Geometry : public AbstractGeometry
    {
    public:
        typedef V vertex_type;
        typedef unsigned int elem_type;
        const static GLenum elem_gl_type = GL_UNSIGNED_INT;
        typedef std::vector<vertex_type> vertex_array_type;
        typedef std::vector<elem_type> elem_array_type;
        typedef std::map<std::string, VertexDesc> attr_map_type;
        typedef std::unique_ptr<Geometry<V>> uptr_type;
        typedef std::shared_ptr<Geometry<V>> sptr_type;
        typedef std::weak_ptr<Geometry<V>> wptr_type;

        Geometry();
        Geometry(Geometry<V> &other);
        Geometry(Geometry<V> &&other);
        virtual ~Geometry();

        virtual AbstractGeometry& operator=(const AbstractGeometry &other);
        virtual AbstractGeometry& operator=(AbstractGeometry &&other);

        void setVertexData(const elem_array_type &new_elems, const vertex_array_type &new_verts);
        void setVertexData(elem_array_type &&new_elems, vertex_array_type &&new_verts);
        void setVertexData(
            const elem_type *const new_elems, unsigned int num_elems,
            const vertex_type *const new_verts, unsigned int num_verts);

        virtual void createBuffers();
        virtual void createVertexArray(const Shader &shader);

        void render() const;

    protected:
        vertex_array_type m_vertices;
        elem_array_type m_elems;
        attr_map_type m_attr_infos;
    };

    struct PCNVertex {
        float position[3];
        float color[4];
        float normal[3];
    };

    // Geometry factory functions.
    Geometry<PCNVertex>::sptr_type makeOctohedronGeometry();
    Geometry<PCNVertex>::sptr_type makeSphereGeometry();

    // Helper functions.
    GLuint duplicateBuffer(GLenum target, GLuint src);
    GLuint duplicateVertexArrayObject(GLuint src);
};

#include "Geometry.tmpl.cpp"

#endif
