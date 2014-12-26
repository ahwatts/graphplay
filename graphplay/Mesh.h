// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_MESH_H_
#define _GRAPHPLAY_GRAPHPLAY_MESH_H_

#include <glm/mat4x4.hpp>
#include <memory>

#include "Geometry.h"
#include "Material.h"

namespace graphplay {
    class Mesh
    {
    public:
        Mesh();
        Mesh(sp_Geometry geo, sp_Material mat);

        void setGeometry(sp_Geometry geo);
        void setMaterial(sp_Material mat);

        inline const float* getTransform() const { return m_model_transform; }
        void setTransform(const glm::mat4x4 &new_transform);

        void render(const glm::mat4x4 &projection, const glm::mat4x4 &model_view) const;

        inline const wp_Geometry getGeometry() const { return wp_Geometry(m_geometry); }
        inline const wp_Material getMaterial() const { return wp_Material(m_material); }
    private:
        // glm::mat4x4 m_model_transform;
        float m_model_transform[16];
        sp_Geometry m_geometry;
        sp_Material m_material;
    };

    typedef std::unique_ptr<Mesh> up_Mesh;
    typedef std::shared_ptr<Mesh> sp_Mesh;
    typedef std::weak_ptr<Mesh> wp_Mesh;
};

#endif
