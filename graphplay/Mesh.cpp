// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "Mesh.h"

namespace graphplay {
    Mesh::Mesh() : m_geometry(), m_material() { }
    Mesh::Mesh(sp_Geometry geo, sp_Material mat) : m_geometry(geo), m_material(mat) { }

    void Mesh::setGeometry(sp_Geometry geo) { m_geometry = geo; }
    void Mesh::setMaterial(sp_Material mat) { m_material = mat; }

    void Mesh::render(const glm::mat4x4 &projection, const glm::mat4x4 &model_view) const {
        m_geometry->render(projection, model_view, *m_material);
    }
};
