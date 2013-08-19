// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <GL/glew.h>
#include <GL/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <gtest/gtest.h>

#include "../graphplay/Geometry.h"

namespace graphplay {
    TEST(Geometry, DefaultConstructor) {
        Geometry g;

        ASSERT_EQ(0, g.getNumVertices());
        ASSERT_EQ(0, g.getStride());
        ASSERT_GT(0, g.getPositionOffset());
        ASSERT_GT(0, g.getNormalOffset());
        ASSERT_GT(0, g.getColorOffset());
        ASSERT_GT(0, g.getTexCoordOffset());
        ASSERT_EQ(g.begin(), g.end());
    }

    TEST(Geometry, AddVertex) {
        Geometry g;

        g.vertex3f(1.0, 2.0, 3.0);
        g.commitNewVertex();

        ASSERT_EQ(1, g.getNumVertices());
        ASSERT_EQ(3, g.getStride());
        ASSERT_EQ(0, g.getPositionOffset());
        ASSERT_GT(0, g.getNormalOffset());
        ASSERT_GT(0, g.getColorOffset());
        ASSERT_GT(0, g.getTexCoordOffset());

        unsigned int num = 0;
        for (auto v : g) {
            ASSERT_EQ(3, v.size());
            ASSERT_EQ(1.0, v[0]);
            ASSERT_EQ(2.0, v[1]);
            ASSERT_EQ(3.0, v[2]);
            ++num;
        }
        ASSERT_EQ(1, num);
    }

    TEST(Geometry, AddVertexCommitsPreviousVertex) {
        Geometry g;

        g.vertex3f(1.0, 2.0, 3.0);
        g.vertex3f(4.0, 5.0, 6.0);

        ASSERT_EQ(1, g.getNumVertices());
        ASSERT_EQ(3, g.getStride());
        ASSERT_EQ(0, g.getPositionOffset());
        ASSERT_GT(0, g.getNormalOffset());
        ASSERT_GT(0, g.getColorOffset());
        ASSERT_GT(0, g.getTexCoordOffset());

        unsigned int num = 0;
        for (auto v : g) {
            ASSERT_EQ(3, v.size());
            ASSERT_EQ(1.0, v[0]);
            ASSERT_EQ(2.0, v[1]);
            ASSERT_EQ(3.0, v[2]);
            ++num;
        }
        ASSERT_EQ(1, num);
    }

    TEST(Geometry, AddVertexMultipleAttributesPositionNotFirst) {
        Geometry g;

        g.color4f(1.0, 0.5, 1.0, 1.0);
        g.vertex3f(1.0, 2.0, 3.0);
        g.commitNewVertex();

        ASSERT_EQ(1, g.getNumVertices());
        ASSERT_EQ(7, g.getStride());
        ASSERT_EQ(4, g.getPositionOffset());
        ASSERT_EQ(0, g.getColorOffset());
        ASSERT_GT(0, g.getNormalOffset());
        ASSERT_GT(0, g.getTexCoordOffset());

        unsigned int num = 0;
        for (auto v : g) {
            ASSERT_EQ(7, v.size());
            ASSERT_EQ(1.0, v[0]);
            ASSERT_EQ(0.5, v[1]);
            ASSERT_EQ(1.0, v[2]);
            ASSERT_EQ(1.0, v[3]);
            ASSERT_EQ(1.0, v[4]);
            ASSERT_EQ(2.0, v[5]);
            ASSERT_EQ(3.0, v[6]);
            ++num;
        }
        ASSERT_EQ(1, num);
    }

    TEST(Geometry, AddVertexMultipleAttributesCannotChangeAttributes) {
        Geometry g;

        g.vertex3f(1.0, 2.0, 3.0);
        g.vertex3f(4.0, 5.0, 6.0);
        g.color4f(1.0, 0.0, 0.0, 1.0);
        g.commitNewVertex();

        ASSERT_EQ(2, g.getNumVertices());
        ASSERT_EQ(3, g.getStride());
        ASSERT_EQ(0, g.getPositionOffset());
        ASSERT_GT(0, g.getNormalOffset());
        ASSERT_GT(0, g.getColorOffset());
        ASSERT_GT(0, g.getTexCoordOffset());

        unsigned int num = 0;
        for (auto v : g) {
            ASSERT_EQ(3, v.size());
            ++num;
        }
        ASSERT_EQ(2, num);
    }

    TEST(OctohedronGeometry, DefaultConstructor) {
        OctohedronGeometry g;

        ASSERT_EQ(8*3, g.getNumVertices());
        ASSERT_EQ(7, g.getStride());

        ASSERT_LE(0, g.getPositionOffset());
        ASSERT_GT(7, g.getPositionOffset());
        ASSERT_LE(0, g.getColorOffset());
        ASSERT_GT(7, g.getColorOffset());

        ASSERT_GT(0, g.getNormalOffset());
        ASSERT_GT(0, g.getTexCoordOffset());

        unsigned int num = 0;
        for (auto v : g) {
            ASSERT_EQ(7, v.size());
            ++num;
        }
        ASSERT_EQ(8*3, num);
    }

    class GeometryOpenGLTest : public testing::Test {
    protected:
        OctohedronGeometry octohedron;
        GLFWwindow *window;

        void bailout(const std::string &msg) {
            std::cerr << msg << std::endl;
            glfwTerminate();
            std::exit(1);
        }

        virtual void SetUp() {
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

        virtual void TearDown() {
            glfwTerminate();
        }
    };

    TEST_F(GeometryOpenGLTest, GenerateBuffers) {
        octohedron.generateBuffers();

        ASSERT_TRUE(glIsBuffer(octohedron.getArrayBuffer()));
        ASSERT_TRUE(glIsBuffer(octohedron.getElementArrayBuffer()));
    }
};
