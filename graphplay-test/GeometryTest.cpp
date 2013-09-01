// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <gtest/gtest.h>

#include "TestOpenGLContext.h"
#include "../graphplay/Geometry.h"

namespace graphplay {
    TEST(GeometryTest, DefaultConstructor) {
        Geometry g;

        ASSERT_EQ(0, g.getNumVertices());
        ASSERT_EQ(0, g.getStride());
        ASSERT_GT(0, g.getPositionOffset());
        ASSERT_GT(0, g.getNormalOffset());
        ASSERT_GT(0, g.getColorOffset());
        ASSERT_GT(0, g.getTexCoordOffset());
        ASSERT_EQ(g.begin(), g.end());
    }

    TEST(GeometryTest, AddVertex) {
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

    TEST(GeometryTest, AddVertexCommitsPreviousVertex) {
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

    TEST(GeometryTest, AddVertexMultipleAttributesPositionNotFirst) {
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

    TEST(GeometryTest, AddVertexMultipleAttributesCannotChangeAttributes) {
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

    TEST(GeometryTest, OctohedronGeometryDefaultConstructor) {
        OctohedronGeometry g;

        ASSERT_EQ(8*3, g.getNumVertices());
        ASSERT_EQ(10, g.getStride());

        ASSERT_LE(0, g.getPositionOffset());
        ASSERT_GT(10, g.getPositionOffset());
        ASSERT_LE(0, g.getNormalOffset());
        ASSERT_GT(10, g.getNormalOffset());
        ASSERT_LE(0, g.getColorOffset());
        ASSERT_GT(10, g.getColorOffset());

        ASSERT_GT(0, g.getTexCoordOffset());

        unsigned int num = 0;
        for (auto v : g) {
            ASSERT_EQ(10, v.size());
            ++num;
        }
        ASSERT_EQ(8*3, num);
    }

    class GeometryTestWithContext : public TestOpenGLContext { };

    TEST_F(GeometryTestWithContext, BufferLifecycle) {
        OctohedronGeometry octohedron;
        octohedron.generateBuffers();

        ASSERT_EQ(GL_TRUE, glIsBuffer(octohedron.getArrayBuffer()));
        ASSERT_EQ(GL_TRUE, glIsBuffer(octohedron.getElementArrayBuffer()));

        octohedron.destroyBuffers();

        ASSERT_EQ(GL_FALSE, glIsBuffer(octohedron.getArrayBuffer()));
        ASSERT_EQ(GL_FALSE, glIsBuffer(octohedron.getElementArrayBuffer()));
    }

    TEST_F(GeometryTestWithContext, Destructor) {
        Geometry *g = new OctohedronGeometry();
        g->generateBuffers();
        GLuint ab = g->getArrayBuffer();
        GLuint eab = g->getElementArrayBuffer();
        delete g;
        g = nullptr;

        ASSERT_EQ(GL_FALSE, glIsBuffer(ab));
        ASSERT_EQ(GL_FALSE, glIsBuffer(eab));
    }
};
