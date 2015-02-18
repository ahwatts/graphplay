// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "graphplay.h"
#include "Mesh.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

namespace graphplay {
    Mesh::Mesh()
        : m_model_transform(),
          m_geometry(),
          m_program()
    {}

    Mesh::Mesh(AbstractGeometry::sptr_type geo, Program::sptr_type program)
        : m_model_transform(),
          m_geometry(geo),
          m_program(program)
    {
        m_geometry->createVertexArray(*m_program);
    }

    Mesh::~Mesh() {}

    void Mesh::setGeometry(AbstractGeometry::sptr_type geo) {
        m_geometry = geo;

        if (m_program) {
            m_geometry->createVertexArray(*m_program);
        }
    }

    void Mesh::setProgram(Program::sptr_type program) {
        m_program = program;

        if (m_geometry) {
            m_geometry->createVertexArray(*m_program);
        }
    }

    void Mesh::setTransform(const glm::mat4x4 &new_transform) {
        m_model_transform = new_transform;
    }

    void Mesh::render() const {
        const IndexMap &unifs = m_program->getUniforms();

        glUseProgram(m_program->getProgramId());
        auto tf_elem = unifs.find("model");
        if (tf_elem != unifs.end()) {
            glUniformMatrix4fv(tf_elem->second, 1, GL_FALSE, glm::value_ptr(m_model_transform));
        }

        tf_elem = unifs.find("model_inv_trans_3");
        if (tf_elem != unifs.end()) {
            glm::mat3x3 model_inv_trans_3 = glm::inverseTranspose(glm::mat3x3(m_model_transform));
            glUniformMatrix3fv(tf_elem->second, 1, GL_FALSE, glm::value_ptr(model_inv_trans_3));
        }

        m_geometry->render();

        glUseProgram(0);
    }
};
