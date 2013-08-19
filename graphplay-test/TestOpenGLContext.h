// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <string>
#include <gtest/gtest.h>

struct GLFWwindow;

namespace graphplay {
    class TestOpenGLContext : public testing::Test {
    protected:
        GLFWwindow *window;

        virtual void SetUp();
        virtual void TearDown();

        void bailout(const std::string &msg);
    };
};
