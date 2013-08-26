// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <gtest/gtest.h>

#include "../graphplay/Camera.h"

namespace graphplay {
    TEST(CameraTest, DefaultConstructor) {
        Camera c;
        ASSERT_EQ(glm::mat4x4(), c.getViewTransform());
    }

    TEST(CameraTest, FullSetConstructor) {
        Camera c(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        glm::mat4x4 expected = glm::translate(glm::mat4x4(), glm::vec3(0, 0, -3));
        ASSERT_EQ(expected, c.getViewTransform());
    }
};
