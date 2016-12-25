// -*- mode: c++; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_FZX_CONSTRAINT_H_
#define _GRAPHPLAY_GRAPHPLAY_FZX_CONSTRAINT_H_

#include "../graphplay.h"

#include <glm/vec3.hpp>

namespace graphplay {
    class Body;

    class AttachedSpring {
    public:
        AttachedSpring(float spring_const, const Body &other);
        const Body& attachedTo() const;
        float springConstant() const;
        glm::vec3 force(const glm::vec3 &self_pos, const glm::vec3 &other_pos) const;

    private:
        const Body &m_other;
        float m_spring_const;
    };
}

#endif
