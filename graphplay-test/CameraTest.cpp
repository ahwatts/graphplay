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
        glm::mat4x4 identity = glm::mat4x4(), camera_transform = c.getViewTransform();
        const float *id_ptr = glm::value_ptr(identity), *camera_transform_ptr = glm::value_ptr(camera_transform);
        ASSERT_EQ(id_ptr, camera_transform_ptr);
    }

    TEST(CameraTest, FullSetConstructor) {
        Camera c(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        glm::mat4x4 expected = glm::translate(glm::mat4x4(), glm::vec3(0, 0, -3));
        glm::mat4x4 actual = c.getViewTransform();
        const float *expected_ptr = glm::value_ptr(expected);
        const float *actual_ptr = glm::value_ptr(actual);
        ASSERT_EQ(expected_ptr, actual_ptr);
    }
};
