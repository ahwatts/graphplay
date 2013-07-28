// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_MESH_H_
#define _GRAPHPLAY_GRAPHPLAY_MESH_H_

#include <map>
#include <string>
#include <vector>
#include <GL/glew.h>

namespace graphplay {
    struct AttrInfo
    {
        std::string name;
        int offset;
        int width;
    };

    bool sortAttrInfosByOffset(const AttrInfo &ai1, const AttrInfo &ai2);

    class Mesh
    {
    public:
        Mesh();
        Mesh(const Mesh &other);
        ~Mesh();

        void addAttribute(std::string &attr_name, int num_vals);
        void initSoup();
        void initSoupGLBuffer();

        void setVertex(int vindex, const std::string &attr_name, const float *data);
        void setFullVertex(int vindex, const float *data);

        inline void setNumTris(int num_tris) { m_num_tris = num_tris; m_num_verts = num_tris * m_verts_per_tri; }
        inline int getNumTris() const { return m_num_tris; }
        inline int getNumVerts() const { return m_num_verts; }

        inline int getVertsPerTri() const { return m_verts_per_tri; }
        inline int getAttrsPerVert() const { return m_attrs_per_vert; }
        inline int getValsPerVert() const { return m_vals_per_vert; }

        inline const float* getSoup() const { return m_soup; }
        inline GLuint getSoupGLBuffer() const { return m_soup_buffer; }
        inline int getSoupSizeInVals() const { return m_num_verts*m_vals_per_vert; }
        inline int getSoupSizeInBytes() const { return getSoupSizeInVals()*sizeof(float); }

        const AttrInfo *getAttrInfo(const std::string &attr_name) const;
        void getSortedAttrInfos(std::vector<AttrInfo> &out) const;

        void setVertexAttribPointer(GLuint location, const std::string &attr_name);

        void dump();

    protected:
        int m_num_tris, m_num_verts;
        int m_verts_per_tri, m_attrs_per_vert, m_vals_per_vert;
        std::map<const std::string, AttrInfo> m_attr_infos;
        float *m_soup;
        GLuint m_soup_buffer;
    };
};

#endif
