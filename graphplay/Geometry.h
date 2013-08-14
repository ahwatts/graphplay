// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_GEOMETRY_H_
#define _GRAPHPLAY_GRAPHPLAY_GEOMETRY_H_

#include <GL/glew.h>
#include <memory>
#include <vector>

#include "graphplay.h"

// Do a forward declaration of MeshGeometry to reduce header
// pollution.
/*namespace collada {
    class MeshGeometry;
};*/

namespace graphplay {
    class Geometry
    {
    public:
        Geometry();
        //Geometry(const collada::MeshGeometry &mesh_geo);

        // These control the data on the GPU.
        void generateBuffers();
        void destroyBuffers();

        GLuint getArrayBuffer() const { return m_data_buffer; }
        GLuint getElementBuffer() const { return m_element_buffer; }

        GLvoid *getPositionOffset() const { return BUFFER_OFFSET_BYTES(m_position_offset*sizeof(float)); }
        GLvoid *getColorOffset() const { return BUFFER_OFFSET_BYTES(m_color_offset*sizeof(float)); }
        unsigned int getStride() const { return m_stride; }
        unsigned int getNumVertices() const { return m_vertex_elems.size(); }

        // This loads from the result of loading a Collada asset XML file.
        //void loadFromCollada(const collada::MeshGeometry &mesh_geo);

        // These work sort of like OpenGL's immediate-vertex functions, if you
        // want to generate the geometry directly. You'll need to call 
        // generateBuffers() after calling them.
        void vertex3f(float x, float y, float z);
        void normal3f(float x, float y, float z);
        void color4f(float r, float g, float b, float a);
        void texCoord2f(float s, float t);
        
        void clearVertices();

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
    };

    class OctohedronGeometry : public Geometry
    {
    public:
        OctohedronGeometry();
    };

    typedef std::shared_ptr<Geometry> sp_Geometry;
    typedef std::weak_ptr<Geometry> wp_Geometry;
};

#endif
