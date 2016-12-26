// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "../../graphplay/graphplay.h"
#include "../../graphplay/fzx/Body.h"

#include <iostream>

#include <glm/gtc/ulp.hpp>
#include <glm/gtx/io.hpp>
#include <gtest/gtest.h>

// #define EXPECT_FLOAT_VEC_EQ(v1, v2) glm::all(glm::lessThanEqual(glm::float_distance((v1), (v2)), { 4, 4, 4 }))
#define EXPECT_FLOAT_VEC_EQ(v1, v2) \
    EXPECT_FLOAT_EQ((v1).x, (v2).x); \
    EXPECT_FLOAT_EQ((v1).y, (v2).y); \
    EXPECT_FLOAT_EQ((v1).z, (v2).z)

namespace graphplay {
    namespace fzx {
        const glm::vec3 ZERO_VEC(0, 0, 0);

        TEST(BodyTest, Mass) {
            Body b;
            EXPECT_FLOAT_EQ(1.0, b.mass());
            b.mass(2);
            EXPECT_EQ(2, b.mass());
        }

        TEST(BodyTest, OnlyPositiveMass) {
            Body b;
            b.mass(-2);
            EXPECT_FLOAT_EQ(1.0, b.mass());

            b.mass(0);
            EXPECT_FLOAT_EQ(1.0, b.mass());
        }

        TEST(BodyTest, Position) {
            Body b;
            EXPECT_EQ(ZERO_VEC, b.position());

            glm::vec3 new_pos(1.0, 2.0, 3.0);
            b.position(new_pos);
            EXPECT_FLOAT_VEC_EQ(new_pos, b.position());
        }

        TEST(BodyTest, Velocity) {
            Body b;
            EXPECT_EQ(ZERO_VEC, b.velocity());

            glm::vec3 new_vel(1.0, 2.0, 3.0);
            b.velocity(new_vel);
            EXPECT_FLOAT_VEC_EQ(new_vel, b.velocity());
        }

        TEST(BodyTest, Force) {
            Body b;
            EXPECT_EQ(ZERO_VEC, b.netForce());

            glm::vec3 push(1.0, 1.3, 1.0);
            b.addForce(push);
            EXPECT_FLOAT_VEC_EQ(push, b.netForce());

            b.addForce(push);
            EXPECT_FLOAT_VEC_EQ(push * 2.0f, b.netForce());

            b.addForce(push * -2.0f);
            EXPECT_FLOAT_VEC_EQ(ZERO_VEC, b.netForce());
        }

        TEST(BodyTest, UpdateAtRest) {
            Body b;
            EXPECT_EQ(ZERO_VEC, b.velocity());
            EXPECT_EQ(ZERO_VEC, b.netForce());

            glm::vec3 pos = b.position();
            b.update(0.1f);
            EXPECT_FLOAT_VEC_EQ(pos, b.position());

            pos = { 0.7, 1.9, -8.2 };
            b.position(pos);
            b.update(0.1f);
            EXPECT_FLOAT_VEC_EQ(pos, b.position());
        }

        TEST(BodyTest, UpdateConstantVelocity) {
            Body b;
            EXPECT_EQ(ZERO_VEC, b.netForce());

            glm::vec3 pos = b.position();
            glm::vec3 vel(1.1, -25.3, 0.0);
            b.velocity(vel);

            b.update(0.1f);
            pos += vel * 0.1f;

            EXPECT_FLOAT_VEC_EQ(pos, b.position());
            EXPECT_FLOAT_VEC_EQ(vel, b.velocity());
        }

        TEST(BodyTest, UpdateAccelerating) {
            Body b;

            glm::vec3 pos = b.position();
            glm::vec3 vel = b.velocity();

            float time_step = 0.1f;
            glm::vec3 push(0.0, -10.0, 21.0);
            glm::vec3 accel = push / b.mass();
            glm::vec3 dvel = accel * time_step;
            glm::vec3 dpos = accel * time_step * time_step * 0.5f + vel * time_step;

            b.addForce(push);
            b.update(time_step);

            EXPECT_FLOAT_VEC_EQ(vel + dvel, b.velocity());
            EXPECT_FLOAT_VEC_EQ(pos + dpos, b.position());
            EXPECT_FLOAT_VEC_EQ(ZERO_VEC, b.netForce());
        }
    }
}
