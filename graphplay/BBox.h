// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_BBOX_H_
#define _GRAPHPLAY_GRAPHPLAY_BBOX_H_

#include <array>
#include <limits>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace graphplay {
    struct PCNVertex;

    class BBox {
    public:
        BBox();
        BBox(glm::vec3 min, glm::vec3 max);
        ~BBox();

        template <typename I>
        static BBox fromVertices(I first, I last) {
            static_assert(
                std::is_same<typename I::value_type, PCNVertex>::value,
                "The BBox fromVertices factory function can only be called with iterators over PCNVertex's.");

            BBox rv;
            for (I i = first; i != last; ++i) {
                for (unsigned int j = 0; j < 3; ++j) {
                    if (i->position[j] < rv.min[j]) {
                        rv.min[j] = i->position[j];
                    }

                    if (i->position[j] > rv.max[j]) {
                        rv.max[j] = i->position[j];
                    }
                }
            }
            return rv;
        }

        template <typename I>
        static BBox fromVectors(I first, I last) {
            BBox rv;
            for (I i = first; i != last; ++i) {
                for (unsigned int j = 0; j < 3; ++j) {
                    if ((*i)[j] < rv.min[j]) {
                        rv.min[j] = (*i)[j];
                    }

                    if ((*i)[j] > rv.max[j]) {
                        rv.max[j] = (*i)[j];
                    }
                }
            }
            return rv;
        }

        BBox axisAlignedAfterTransform(const glm::mat4x4 &transform) const;

        glm::vec3 min, max;

    protected:
    };

    // class BBoxGeometry : public MutableGeometry<PCNVertex> {
    // };
}

#endif
