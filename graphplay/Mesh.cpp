// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include "Mesh.h"
#include "graphplay.h"

Mesh::Mesh()
  : m_num_tris(0),
    m_num_verts(0),
    m_verts_per_tri(3),
    m_attrs_per_vert(0),
    m_vals_per_vert(0),
    m_attr_infos(),
    m_soup(0),
    m_soup_buffer(0)
{ }

Mesh::Mesh(const Mesh &other)
  : m_num_tris(other.m_num_tris),
    m_num_verts(other.m_num_verts),
    m_verts_per_tri(other.m_verts_per_tri),
    m_attrs_per_vert(other.m_attrs_per_vert),
    m_vals_per_vert(other.m_vals_per_vert),
    m_attr_infos(other.m_attr_infos),
    m_soup(0),
    m_soup_buffer(0)
{
    if (other.m_soup) {
        m_soup = new float[other.getSoupSizeInVals()];
        memcpy(m_soup, other.m_soup, other.getSoupSizeInBytes());

        if (glIsBuffer(other.m_soup_buffer)) {
            initSoupGLBuffer();
        }
    }
}

Mesh::~Mesh()
{
    if (m_soup) {
        delete [] m_soup;
    }

    if (glIsBuffer(m_soup_buffer)) {
        glDeleteBuffers(1, &m_soup_buffer);
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

void Mesh::initSoupGLBuffer()
{
    if (glIsBuffer(m_soup_buffer)) {
        glDeleteBuffers(1, &m_soup_buffer);
    }

    glGenBuffers(1, &m_soup_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_soup_buffer);
    glBufferData(GL_ARRAY_BUFFER, getSoupSizeInBytes(), m_soup, GL_STATIC_DRAW);
}

void Mesh::addAttribute(std::string &name, int num_vals)
{
    AttrInfo ai;

    ai.name = name;
    ai.offset = m_vals_per_vert;
    ai.width = num_vals;

    m_attr_infos[name] = ai;
    m_vals_per_vert += num_vals;
    m_attrs_per_vert += 1;
}

void Mesh::setVertex(int vindex, const std::string &attr_name, const float *data)
{
    AttrInfo &attr = m_attr_infos[attr_name];

    if (m_soup) {
        int sindex = vindex*m_vals_per_vert + attr.offset;
        for (int i = 0; i < attr.width; ++i) {
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
    printf("Mesh: %p\n", this);
    printf("  triangles: %d vertices: %d\n", m_num_tris, m_num_verts);
    printf("  vertices per triangle: %d\n", m_verts_per_tri);
    printf("  attributes per vertex: %d\n", m_attrs_per_vert);
    printf("  values per vertex: %d\n", m_vals_per_vert);
    printf("  soup size: %d floats, %lu bytes\n",
           m_vals_per_vert*m_num_verts,
           m_vals_per_vert*m_num_verts*sizeof(float));

    std::vector<AttrInfo> attrs;
    getSortedAttrInfos(attrs);

    for (unsigned int i = 0; i < attrs.size(); ++i) {
        printf("  attribute: %s width = %d offset = %d\n", 
            attrs[i].name.c_str(), attrs[i].width, attrs[i].offset);
    }

    for (int i = 0; i < m_num_verts; ++i) {
        printf("Vertex %3d:", i);
        for (unsigned int j = 0; j < attrs.size(); ++j) {
            printf(" %s: (", attrs[j].name.c_str());
            for (int k = 0; k < attrs[j].width; ++k) {
                printf("%6.3f", m_soup[m_vals_per_vert*i + attrs[j].offset + k]);
                if (k != attrs[j].width - 1) {
                    printf(", ");
                }
            }
            printf(")");
        }
        printf("\n");
    }
}

bool sortAttrInfosByOffset(const AttrInfo &ai1, const AttrInfo &ai2)
{
    return ai1.offset < ai2.offset;
}


const AttrInfo *Mesh::getAttrInfo(const std::string &attr_name) const
{
    return &m_attr_infos.at(attr_name);
}

void Mesh::getSortedAttrInfos(std::vector<AttrInfo> &out) const
{
    std::map<std::string, AttrInfo>::const_iterator it;

    out.clear();
    for (it = m_attr_infos.begin(); it != m_attr_infos.end(); ++it) {
        out.push_back(it->second);
    }
    std::sort(out.begin(), out.end(), sortAttrInfosByOffset);
}

void Mesh::setVertexAttribPointer(GLuint loc, const std::string &attr_name)
{
    const AttrInfo *attr_info = getAttrInfo(attr_name);
    int offset_bytes = attr_info->offset*sizeof(float);

    glEnableVertexAttribArray(loc);
    glBindBuffer(GL_ARRAY_BUFFER, m_soup_buffer);
    glVertexAttribPointer(loc, attr_info->width, GL_FLOAT, GL_FALSE,
        m_vals_per_vert*sizeof(float),
        BUFFER_OFFSET_BYTES(offset_bytes));
}
