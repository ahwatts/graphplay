// -*- mode: c++; indent-tabs-mode: nil -*-

#include "Constraint.h"

namespace graphplay {
    AttachedSpring::AttachedSpring(float spring_const, const Body &other)
        : m_other(other),
          m_spring_const(spring_const)
          
    {}

    const Body& AttachedSpring::attachedTo() const {
        return m_other;
    }

    float AttachedSpring::springConstant() const {
        return m_spring_const;
    }

    glm::vec3 AttachedSpring::force(const glm::vec3 &self_pos, const glm::vec3 &other_pos) const {
        return (other_pos - self_pos) * m_spring_const;
    }
}
