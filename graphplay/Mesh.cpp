// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "Mesh.h"

namespace graphplay {
    Mesh::Mesh() : m_geometry(), m_material() { }
    Mesh::Mesh(sp_Geometry geo, sp_Material mat) : m_geometry(geo), m_material(mat) { }
};
