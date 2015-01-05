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

        static void handle_glfw_error(int code, const char *desc);
        void bailout(const std::string &msg);
    };
};
