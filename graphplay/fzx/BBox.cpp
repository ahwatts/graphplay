// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "../graphplay.h"
#include "BBox.h"

#include <limits>

#include <glm/vec4.hpp>

namespace graphplay {
    namespace fzx {
        static constexpr float FLOAT_MIN = std::numeric_limits<float>::lowest();
        static constexpr float FLOAT_MAX = std::numeric_limits<float>::max();

        BBox::BBox()
            : BBox(glm::vec3{FLOAT_MAX, FLOAT_MAX, FLOAT_MAX},
                   glm::vec3{FLOAT_MIN, FLOAT_MIN, FLOAT_MIN})
        {}

        BBox::BBox(glm::vec3 min, glm::vec3 max)
            : min{min},
              max{max}
        {}
    
        BBox::~BBox() {}

        BBox BBox::axisAlignedAfterTransform(const glm::mat4x4 &transform) const {
            glm::vec4 corners[8] = {
                { min.x, min.y, min.z, 1.0 },
                { min.x, min.y, max.z, 1.0 },
                { min.x, max.y, min.z, 1.0 },
                { min.x, max.y, max.z, 1.0 },
                { max.x, min.y, min.z, 1.0 },
                { max.x, min.y, max.z, 1.0 },
                { max.x, max.y, min.z, 1.0 },
                { max.x, max.y, max.z, 1.0 },
            };

            return BBox::fromVectors(corners, corners + 8);
        }

        bool BBox::collides(const BBox &other) const {
            return (min.x < other.max.x && other.min.x < max.x &&
                    min.y < other.max.y && other.min.y < max.y &&
                    min.z < other.max.z && other.min.z < max.z);
        }
    }
}
