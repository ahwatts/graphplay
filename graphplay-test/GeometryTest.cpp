// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <gtest/gtest.h>

#include "../graphplay/Geometry.h"

namespace graphplay {
    TEST(Geometry, DefaultConstructor) {
        Geometry g;

        ASSERT_EQ(0, g.getNumVertices());
        ASSERT_EQ(-1, g.getPositionOffset());
        ASSERT_EQ(-1, g.getNormalOffset());
        ASSERT_EQ(-1, g.getColorOffset());
        ASSERT_EQ(-1, g.getTexCoordOffset());
        ASSERT_EQ(g.begin(), g.end());
    }
};
