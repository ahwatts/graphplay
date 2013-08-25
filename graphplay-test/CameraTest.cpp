// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <glm/glm.hpp>
#include <gtest/gtest.h>

#include "../graphplay/Camera.h"

namespace graphplay {
    TEST(CameraTest, DefaultConstructor) {
        Camera c;
        ASSERT_EQ(glm::mat4x4(), c.getViewTransform());
    }
};
