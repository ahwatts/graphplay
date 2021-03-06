// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_GFX_GEOMETRY_H_
#define _GRAPHPLAY_GRAPHPLAY_GFX_GEOMETRY_H_

#include "../graphplay.h"

#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "../opengl.h"
// #include "../fzx/BBox.h"

namespace graphplay {
    namespace gfx {
        class Program;

        struct VertexDesc {
            void *offset;
            GLenum type;
            unsigned int count;
        };
        typedef std::map<std::string, VertexDesc> AttrMap;

        class AbstractGeometry {
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

            // const fzx::BBox& boundingBox() const;
            // virtual void updateBoundingBox();

            inline const GLuint vertexBufferId() const { return m_vertex_buffer; }
            inline const GLuint elemBufferId() const { return m_elem_buffer; }
            inline const GLuint vertexArrayObjectId() const { return m_array_object; }

            virtual void createBuffers();
            virtual void deleteBuffers();
            virtual void createVertexArray(const Program &program);
            virtual void deleteVertexArray();

            virtual void render() const;

            GLenum draw_type;

        protected:
            GLuint m_vertex_buffer;
            GLuint m_elem_buffer;
            GLuint m_array_object;
            // fzx::BBox m_bbox;
        };

        template <typename V>
        class Geometry : public AbstractGeometry
        {
        public:
            typedef V vertex_type;
            typedef GLuint elem_type;
            const static GLenum elem_gl_type = GL_UNSIGNED_INT;
            typedef std::vector<vertex_type> vertex_array_type;
            typedef std::vector<elem_type> elem_array_type;
            typedef std::unique_ptr<Geometry<V> > uptr_type;
            typedef std::shared_ptr<Geometry<V> > sptr_type;
            typedef std::weak_ptr<Geometry<V> > wptr_type;

            Geometry();
            Geometry(const Geometry<V> &other);
            Geometry(Geometry<V> &&other);
            virtual ~Geometry();

            template <class ElemIter, class VertIter>
            Geometry(ElemIter first_elem, ElemIter last_elem, VertIter first_vert, VertIter last_vert)
                : AbstractGeometry{},
                  m_vertices{first_vert, last_vert},
                  m_elems{first_elem, last_elem},
                  m_attr_infos{V::description}
            {}

            virtual AbstractGeometry& operator=(const AbstractGeometry &other);
            virtual AbstractGeometry& operator=(AbstractGeometry &&other);

            virtual Geometry<V>&      operator=(const Geometry<V> &other);
            virtual Geometry<V>&      operator=(Geometry<V> &&other);

            // virtual void updateBoundingBox();

            void setVertexData(const elem_array_type &new_elems, const vertex_array_type &new_verts);
            void setVertexData(elem_array_type &&new_elems, vertex_array_type &&new_verts);
            void setVertexData(
                const elem_type *const new_elems, unsigned int num_elems,
                const vertex_type *const new_verts, unsigned int num_verts);

            virtual void createBuffers();
            virtual void createVertexArray(const Program &program);

            inline vertex_array_type& vertices() { return m_vertices; }
            inline elem_array_type& elements() { return m_elems; }
            inline const AttrMap& attrInfos() { return m_attr_infos; }

            void render() const;

        protected:
            vertex_array_type m_vertices;
            elem_array_type m_elems;
            const AttrMap &m_attr_infos;
        };

        // template<typename V>
        // class MutableGeometry : public Geometry<V> {
        // public:
        //     typedef std::unique_ptr<MutableGeometry<V> > uptr_type;
        //     typedef std::shared_ptr<MutableGeometry<V> > sptr_type;
        //     typedef std::weak_ptr<MutableGeometry<V> > wptr_type;

        //     MutableGeometry();
        //     MutableGeometry(const Geometry<V> &other);
        //     MutableGeometry(Geometry<V> &&other);
        //     virtual ~MutableGeometry();

        //     virtual MutableGeometry<V>& operator=(const Geometry<V> &other);
        //     virtual MutableGeometry<V>& operator=(Geometry<V> &&other);

        //     virtual void createBuffers();
        //     virtual void updateBuffers();
        // };

        struct PCNVertex {
            float position[3];
            float color[4];
            float normal[3];
            static const AttrMap description;
        };

        typedef Geometry<PCNVertex> PCNGeometry;

        // Output functions.
        std::ostream& operator<<(std::ostream& stream, const PCNVertex &vertex);

        // Geometry factory functions.
        Geometry<PCNVertex>::sptr_type makeOctohedronGeometry();
        Geometry<PCNVertex>::sptr_type makeIcosahedronGeometry();
        Geometry<PCNVertex>::sptr_type makeSphereGeometry();
        Geometry<PCNVertex>::sptr_type makeWireframeCubeGeometry();
        // MutableGeometry<PCNVertex>::sptr_type makeBoundingBoxGeometry(const fzx::BBox &bbox);
        Geometry<PCNVertex>::sptr_type loadPCNFile(const char *filename);
        Geometry<PCNVertex>::sptr_type loadPlyFile(const char *filename);
    }
}

#include "Geometry.tmpl.cpp"

#endif
