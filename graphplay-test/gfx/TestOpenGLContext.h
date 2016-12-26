// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_TEST_GFX_TEST_OPENGL_CONTEXT_H_
#define _GRAPHPLAY_GRAPHPLAY_TEST_GFX_TEST_OPENGL_CONTEXT_H_

#include <string>

#include <gtest/gtest.h>

struct GLFWwindow;

namespace graphplay {
    namespace gfx {
        class TestOpenGLContext : public testing::Test {
        protected:
            GLFWwindow *window;

            virtual void SetUp();
            virtual void TearDown();

            static void handle_glfw_error(int code, const char *desc);
            void bailout(const std::string &msg);
        };
    }
}

#endif
