// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_MESH_H_
#define _GRAPHPLAY_GRAPHPLAY_MESH_H_

#include "Geometry.h"
#include "Material.h"

namespace graphplay {
    class Mesh
    {
    public:
        Mesh();
        Mesh(sp_Geometry geo, sp_Material mat);
        ~Mesh();

        void setGeometry(sp_Geometry geo);
        void setMaterial(sp_Material mat);

        inline const wp_Geometry getGeometry() const { return wp_Geometry(m_geometry); }
        inline const wp_Material getMaterial() const { return wp_Material(m_material); }
    private:
        sp_Geometry m_geometry;
        sp_Material m_material;
    };
};

#endif
