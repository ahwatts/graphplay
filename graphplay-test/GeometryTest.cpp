// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <gtest/gtest.h>

#include "../graphplay/Geometry.h"

namespace graphplay {
    TEST(Geometry, DefaultConstructor) {
        Geometry g;

        ASSERT_EQ(0, g.getNumVertices());
        ASSERT_EQ(0, g.getStride());
        ASSERT_GT(0, g.getPositionOffset());
        ASSERT_GT(0, g.getNormalOffset());
        ASSERT_GT(0, g.getColorOffset());
        ASSERT_GT(0, g.getTexCoordOffset());
        ASSERT_EQ(g.begin(), g.end());
    }

    TEST(OctohedronGeometry, DefaultConstructor) {
        OctohedronGeometry g;

        ASSERT_EQ(8*3, g.getNumVertices());
        ASSERT_EQ(7, g.getStride());

        ASSERT_LE(0, g.getPositionOffset());
        ASSERT_GT(7, g.getPositionOffset());
        ASSERT_LE(0, g.getColorOffset());
        ASSERT_GT(7, g.getColorOffset());

        ASSERT_GT(0, g.getNormalOffset());
        ASSERT_GT(0, g.getTexCoordOffset());

        unsigned int num = 0;
        for (auto v : g) {
            ASSERT_EQ(7, v.size());
            ++num;
        }
        ASSERT_EQ(8*3, num);
    }
};
