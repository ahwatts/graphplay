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
        typedef std::unique_ptr<Mesh> uptr_type;
        typedef std::shared_ptr<Mesh> sptr_type;
        typedef std::weak_ptr<Mesh> wptr_type;

        Mesh();
        Mesh(const Mesh &other);
        Mesh(AbstractGeometry::sptr_type geo, Shader::sptr_type shader);
        ~Mesh();

        Mesh& operator=(Mesh &other);
        Mesh& operator=(Mesh &&other);

        void setGeometry(AbstractGeometry::sptr_type geo);
        void setShader(Shader::sptr_type shader);

        inline const float* getTransform() const { return m_model_transform; }
        void setTransform(const glm::mat4x4 &new_transform);

        void render() const;

        inline const AbstractGeometry::wptr_type getGeometry() const { return AbstractGeometry::wptr_type(m_geometry); }
        inline const Shader::wptr_type getShader() const { return Shader::wptr_type(m_shader); }
    private:
        // glm::mat4x4 m_model_transform;
        float m_model_transform[16];
        AbstractGeometry::sptr_type m_geometry;
        Shader::sptr_type m_shader;
    };
};

#endif
