// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <gtest/gtest.h>

#include "../graphplay/Geometry.h"

namespace graphplay {
    TEST(Geometry, DefaultConstructor) {
        Geometry g;
        ASSERT_EQ(0, g.m_stride);
    }
};
