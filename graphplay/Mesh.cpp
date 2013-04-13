// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <algorithm>
#include <stdio.h>
#include <string.h>

#include "Mesh.h"

Mesh::Mesh()
  : m_num_tris(0),
    m_num_verts(0),
    m_verts_per_tri(3),
    m_attrs_per_vert(0),
    m_vals_per_vert(0),
    m_attr_offsets(),
    m_attr_widths(),
    m_soup(0)
{ }

Mesh::Mesh(const Mesh &other)
  : m_num_tris(other.m_num_tris),
    m_num_verts(other.m_num_verts),
    m_verts_per_tri(other.m_verts_per_tri),
    m_attrs_per_vert(other.m_attrs_per_vert),
    m_vals_per_vert(other.m_vals_per_vert),
    m_attr_offsets(other.m_attr_offsets),
    m_attr_widths(other.m_attr_widths),
    m_soup(0)
{
    if (other.m_soup) {
        int soup_size = other.m_num_verts*other.m_vals_per_vert;
        m_soup = new float[soup_size];
        memcpy(m_soup, other.m_soup, soup_size*sizeof(float));
    }
}

Mesh::~Mesh()
{
    if (m_soup) {
        delete [] m_soup;
    }
}

void Mesh::initSoup()
{
    if (m_soup) {
        delete [] m_soup;
    }

    int soup_size = m_num_verts*m_vals_per_vert;
    m_soup = new float[soup_size*sizeof(float)];
}

void Mesh::addAttribute(std::string &name, int num_vals)
{
    m_attr_offsets[name] = m_vals_per_vert;
    m_attr_widths[name] = num_vals;
    m_vals_per_vert += num_vals;
    m_attrs_per_vert += 1;
}

void Mesh::setVertex(int vindex, const std::string &attr_name, const float *data)
{
    if (m_soup) {
        int sindex = vindex*m_vals_per_vert + m_attr_offsets[attr_name];
        for (int i = 0; i < m_attr_widths[attr_name]; ++i) {
            m_soup[sindex+i] = data[i];
        }
    }
}

void Mesh::setFullVertex(int vindex, const float *data)
{
    if (m_soup) {
        int sindex = vindex*m_vals_per_vert;
        for (int i = 0; i < m_vals_per_vert; ++i) {
            m_soup[sindex+i] = data[i];
        }
    }
}

void Mesh::dump()
{
    std::map<std::string, int>::const_iterator it;

    printf("Mesh: %p\n", this);
    printf("  triangles: %d vertices: %d\n", m_num_tris, m_num_verts);
    printf("  vertices per triangle: %d\n", m_verts_per_tri);
    printf("  attributes per vertex: %d\n", m_attrs_per_vert);
    printf("  values per vertex: %d\n", m_vals_per_vert);
    printf("  soup size: %d floats, %'lu bytes\n",
           m_vals_per_vert*m_num_verts,
           m_vals_per_vert*m_num_verts*sizeof(float));

    for (it = m_attr_offsets.begin(); it != m_attr_offsets.end(); ++it) {
        const std::string &attr_name = it->first;
        const int &offset = it->second, &width = m_attr_widths[attr_name];
        printf("  attribute: %s width = %d offset = %d\n", attr_name.c_str(), width, offset);
    }

    for (int i = 0; i < m_num_verts; ++i) {
        printf("Vertex %3d:", i);
        for (it = m_attr_offsets.begin(); it != m_attr_offsets.end(); ++it) {
            const std::string &attr_name = it->first;
            const int &offset = it->second, &width = m_attr_widths[attr_name];
            printf(" %s: (", attr_name.c_str());
            for (int j = 0; j < width; ++j) {
                printf("%6.3f", m_soup[m_vals_per_vert*i + offset + j]);
                if (j != width - 1) {
                    printf(", ");
                }
            }
            printf(")");
        }
        printf("\n");
    }
}

bool sortAttrInfos(AttrInfo &ai1, AttrInfo &ai2)
{
    return ai1.offset < ai2.offset;
}

void Mesh::getAttrInfo(std::vector<AttrInfo> &out) const
{
    out.clear();

    for (std::map<std::string, int>::const_iterator it = m_attr_offsets.begin(); it != m_attr_offsets.end(); ++it) {
        AttrInfo a;
        a.name = it->first;
        a.offset = it->second;
        a.width = m_attr_widths.at(it->first);
        out.push_back(a);
    }

    std::sort(out.begin(), out.end(), sortAttrInfos);
}
