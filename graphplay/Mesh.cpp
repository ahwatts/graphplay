// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "graphplay.h"
#include "Mesh.h"
#include <glm/gtc/type_ptr.hpp>

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
        auto model_tf_elem = unifs.find("model");
        if (model_tf_elem != unifs.end()) {
            glUniformMatrix4fv(model_tf_elem->second, 1, GL_FALSE, glm::value_ptr(m_model_transform));
        }

        m_geometry->render();

        glUseProgram(0);
    }
};
