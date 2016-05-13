// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_MESH_H_
#define _GRAPHPLAY_GRAPHPLAY_MESH_H_

#include <glm/mat4x4.hpp>
#include <memory>

#include "Geometry.h"
#include "Shader.h"

namespace graphplay {
    class Mesh
    {
    public:
        typedef std::unique_ptr<Mesh> uptr_type;
        typedef std::shared_ptr<Mesh> sptr_type;
        typedef std::weak_ptr<Mesh> wptr_type;

        Mesh();
        Mesh(AbstractGeometry::sptr_type geo, Program::sptr_type program);

        void geometry(AbstractGeometry::sptr_type geo);
        inline const AbstractGeometry::wptr_type geometry() const { return AbstractGeometry::wptr_type(m_geometry); }

        void program(Program::sptr_type program);
        inline const Program::wptr_type program() const { return Program::wptr_type(m_program); }

        void modelTransformation(const glm::mat4x4 &new_transform);
        inline const glm::mat4x4& modelTransformation() const { return m_model_transform; }

        void render() const;

    private:
        glm::mat4x4 m_model_transform;
        AbstractGeometry::sptr_type m_geometry;
        Program::sptr_type m_program;
    };
};

#endif
