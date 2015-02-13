// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_GEOMETRY_H_
#define _GRAPHPLAY_GRAPHPLAY_GEOMETRY_H_

#include <map>
#include <memory>
#include <vector>

#include "graphplay.h"
#include "opengl.h"

namespace graphplay {
    class Program;

    struct VertexDesc
    {
        void *offset;
        GLenum type;
        unsigned int count;
    };
    typedef std::map<std::string, VertexDesc> AttrMap;

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

        virtual AbstractGeometry& operator=(const AbstractGeometry &other);
        virtual AbstractGeometry& operator=(AbstractGeometry &&other);

        inline const GLuint getVertexBufferId() const { return m_vertex_buffer; }
        inline const GLuint getElemBufferId() const { return m_elem_buffer; }
        inline const GLuint getVertexArrayObjectId() const { return m_array_object; }

        virtual void createBuffers();
        virtual void deleteBuffers();
        virtual void createVertexArray(const Program &program);
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
        typedef std::unique_ptr<Geometry<V>> uptr_type;
        typedef std::shared_ptr<Geometry<V>> sptr_type;
        typedef std::weak_ptr<Geometry<V>> wptr_type;

        Geometry();
        Geometry(const Geometry<V> &other);
        Geometry(Geometry<V> &&other);
        virtual ~Geometry();

        virtual AbstractGeometry& operator=(const AbstractGeometry &other);
        virtual Geometry<V>& operator=(const Geometry<V> &other);
        virtual AbstractGeometry& operator=(AbstractGeometry &&other);
        virtual Geometry<V>& operator=(Geometry<V> &&other);

        void setVertexData(const elem_array_type &new_elems, const vertex_array_type &new_verts);
        void setVertexData(elem_array_type &&new_elems, vertex_array_type &&new_verts);
        void setVertexData(
            const elem_type *const new_elems, unsigned int num_elems,
            const vertex_type *const new_verts, unsigned int num_verts);

        virtual void createBuffers();
        virtual void createVertexArray(const Program &program);

        inline vertex_array_type& getVertices() { return m_vertices; }
        inline elem_array_type& getElems() { return m_elems; }
        inline const AttrMap& getAttrInfo() { return m_attr_infos; }

        void render() const;

    protected:
        vertex_array_type m_vertices;
        elem_array_type m_elems;
        const AttrMap &m_attr_infos;
    };

    struct PCNVertex {
        float position[3];
        float color[4];
        float normal[3];
        static const AttrMap description;
    };

    // Geometry factory functions.
    Geometry<PCNVertex>::sptr_type makeOctohedronGeometry();
    Geometry<PCNVertex>::sptr_type makeSphereGeometry();
};

#include "Geometry.tmpl.cpp"

#endif
