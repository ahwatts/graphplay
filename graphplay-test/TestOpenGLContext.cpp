// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <GL/glew.h>
#include <GL/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <gtest/gtest.h>

#include "TestOpenGLContext.h"

namespace graphplay {
    void TestOpenGLContext::SetUp() {
        if (!glfwInit()) {
            bailout("Could not initialize GLFW!");
        }

        window = glfwCreateWindow(640, 480, "Graphplay Test Window", NULL, NULL);

        if (!window) {
            bailout("Could not create window!");
        }

        glfwMakeContextCurrent(window);

        GLenum glew_err = glewInit();
        if (glew_err != GLEW_OK) {
            std::ostringstream msg;
            msg << "Could not initialize GLEW: " << glewGetErrorString(glew_err);
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
};
