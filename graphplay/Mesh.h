// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _MESH_H_
#define _MESH_H_

#include <map>
#include <string>

class Mesh
{
public:
    Mesh();
    Mesh(const Mesh &other);
    ~Mesh();

    void addAttribute(std::string &attr_name, int num_vals);
    void initSoup();

    void setVertex(int vindex, const std::string &attr_name, const float *data);
    void setFullVertex(int vindex, const float *data);

    inline void setNumTris(int num_tris) { m_num_tris = num_tris; m_num_verts = num_tris * m_verts_per_tri; }
    inline int getNumTris() const { return m_num_tris; }
    inline int getNumVerts() const { return m_num_verts; }

    inline int getVertsPerTri() const { return m_verts_per_tri; }
    inline int getAttrsPerVert() const { return m_attrs_per_vert; }
    inline int getValsPerVert() const { return m_vals_per_vert; }

    void dump();

private:
    int m_num_tris, m_num_verts;
    int m_verts_per_tri, m_attrs_per_vert, m_vals_per_vert;
    std::map<std::string, int> m_attr_offsets, m_attr_widths;
    float *m_soup;
};

#endif
