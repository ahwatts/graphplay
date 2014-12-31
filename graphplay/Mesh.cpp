// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "graphplay.h"
#include "Mesh.h"
#include <glm/gtc/type_ptr.hpp>

namespace graphplay {
    void copy_mat4x4_to_array(float *arr, const glm::mat4x4 &mat) {
        const float *ptr = glm::value_ptr(mat);
        unsigned int i = 0;
        for (i = 0; i < 16; ++i) {
            arr[i] = ptr[i];
        }
    }

    Mesh::Mesh() : m_geometry(), m_material()
    {
        copy_mat4x4_to_array(m_model_transform, glm::mat4x4());
    }
    
    Mesh::Mesh(sp_Geometry geo, sp_Material mat) : m_geometry(geo), m_material(mat)
    {
        copy_mat4x4_to_array(m_model_transform, glm::mat4x4());
        m_geometry->setUpVertexArray(*m_material);
    }

    void Mesh::setGeometry(sp_Geometry geo) {
        m_geometry = geo;

        if (m_material) {
            m_geometry->setUpVertexArray(*m_material);
        }
    }

    void Mesh::setMaterial(sp_Material mat) {
        m_material = mat;

        if (m_geometry) {
            m_geometry->setUpVertexArray(*m_material);
        }
    }

    void Mesh::setTransform(const glm::mat4x4 &new_transform) {
        copy_mat4x4_to_array(m_model_transform, new_transform);
    }

    void Mesh::render(const glm::mat4x4 &projection, const glm::mat4x4 &model_view) const {
        glm::mat4x4 transform_mat = glm::make_mat4x4(m_model_transform);
        glm::mat4x4 full_mv = model_view * transform_mat;
        m_geometry->render(projection, full_mv, *m_material);
    }
};
