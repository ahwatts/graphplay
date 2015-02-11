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

    Mesh::Mesh() : m_geometry(), m_shader()
    {
        copy_mat4x4_to_array(m_model_transform, glm::mat4x4());
    }
    
    Mesh::Mesh(AbstractGeometry::sptr_type geo, Shader::sptr_type shader) : m_geometry(geo), m_shader(shader)
    {
        copy_mat4x4_to_array(m_model_transform, glm::mat4x4());
        m_geometry->createVertexArray(*m_shader);
    }

    Mesh::~Mesh() {}

    void Mesh::setGeometry(AbstractGeometry::sptr_type geo) {
        m_geometry = geo;

        if (m_shader) {
            m_geometry->createVertexArray(*m_shader);
        }
    }

    void Mesh::setShader(Shader::sptr_type shader) {
        m_shader = shader;

        if (m_geometry) {
            m_geometry->createVertexArray(*m_shader);
        }
    }

    void Mesh::setTransform(const glm::mat4x4 &new_transform) {
        copy_mat4x4_to_array(m_model_transform, new_transform);
    }

    void Mesh::render() const {
        // glm::mat4x4 model = glm::make_mat4x4(m_model_transform);
        m_geometry->render();
    }
};
