// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <gtest/gtest.h>

#include "../graphplay/Camera.h"

namespace graphplay {
    TEST(CameraTest, DefaultConstructor) {
        Camera c;
        unsigned int i = 0;
        glm::mat4x4 identity = glm::mat4x4();
        glm::mat4x4 camera_transform = c.getViewTransform();
        const float *identity_ptr = glm::value_ptr(identity);
        const float *camera_transform_ptr = glm::value_ptr(camera_transform);
        for (i = 0; i < 16; ++i) {
            ASSERT_FLOAT_EQ(identity_ptr[i], camera_transform_ptr[i]);
        }
    }

    TEST(CameraTest, FullSetConstructor) {
        Camera c(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        unsigned int i = 0;
        glm::mat4x4 expected = glm::translate(glm::mat4x4(), glm::vec3(0, 0, -3));
        glm::mat4x4 actual = c.getViewTransform();
        const float *expected_ptr = glm::value_ptr(expected);
        const float *actual_ptr = glm::value_ptr(actual);
        for (i = 0; i < 16; ++i) {
            ASSERT_FLOAT_EQ(expected_ptr[i], actual_ptr[i]);
        }
    }
};
