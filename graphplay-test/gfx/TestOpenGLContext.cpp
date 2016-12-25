// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "../graphplay/graphplay.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <gtest/gtest.h>

#include "TestOpenGLContext.h"

#include "../graphplay/opengl.h"

namespace graphplay {
    void TestOpenGLContext::SetUp() {
        glfwSetErrorCallback(TestOpenGLContext::handle_glfw_error);
        if (!glfwInit()) {
            bailout("Could not initialize GLFW!");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
        window = glfwCreateWindow(640, 480, "Graphplay Test Window", NULL, NULL);

        if (!window) {
            bailout("Could not create window!");
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::ostringstream msg;
            msg << "Could not initialize OpenGL context." << std::endl;
            bailout(msg.str());
        }
    }

    void TestOpenGLContext::TearDown() {
        glfwTerminate();
    }

    void TestOpenGLContext::bailout(const std::string &msg) {
        std::cerr << msg << std::endl;
        glfwTerminate();
        std::exit(1);
    }

    void TestOpenGLContext::handle_glfw_error(int code, const char *desc) {
        std::cerr << "GLFW Error Code " << code << std::endl
                  << desc << std::endl;
    }
};
