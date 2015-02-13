// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "../graphplay/graphplay.h"

#include <gtest/gtest.h>

#include "TestOpenGLContext.h"
#include "../graphplay/Geometry.h"

namespace graphplay {
    class GeometryTest : public TestOpenGLContext {};

    void assertNoBuffersCreated(const AbstractGeometry &g) {
        ASSERT_EQ(0, g.getElemBufferId());
        ASSERT_EQ(GL_FALSE, glIsBuffer(g.getElemBufferId()));
        ASSERT_EQ(GL_NO_ERROR, glGetError());

        ASSERT_EQ(0, g.getVertexBufferId());
        ASSERT_EQ(GL_FALSE, glIsBuffer(g.getVertexBufferId()));
        ASSERT_EQ(GL_NO_ERROR, glGetError());

        ASSERT_EQ(0, g.getVertexArrayObjectId());
        ASSERT_EQ(GL_FALSE, glIsVertexArray(g.getVertexArrayObjectId()));
        ASSERT_EQ(GL_NO_ERROR, glGetError());
    }

    TEST_F(GeometryTest, AbstractGeometryDefaultConstructor) {
        AbstractGeometry g;
        assertNoBuffersCreated(g);
    }

    TEST_F(GeometryTest, AbstractGeometryCreateBuffers) {
        AbstractGeometry g;
        g.createBuffers();
        assertNoBuffersCreated(g);
    }

    TEST_F(GeometryTest, AbstractGeometryDeleteBuffers) {
        AbstractGeometry g;
        g.deleteBuffers();
        assertNoBuffersCreated(g);
    }

    TEST_F(GeometryTest, RealGeometryDefaultConstructor) {
        Geometry<PCNVertex> g;

        assertNoBuffersCreated(dynamic_cast<AbstractGeometry&>(g));
        ASSERT_TRUE(g.getVertices().empty());
        ASSERT_TRUE(g.getElems().empty());
        ASSERT_EQ(&PCNVertex::description, &g.getAttrInfo());
    }

    const Geometry<PCNVertex>::vertex_array_type verts = {
        { { 0, 0, 1 }, { 0, 0, 1, 1 }, { 0.577, 0.577, 0.577 } },
        { { 1, 0, 0 }, { 1, 0, 0, 1 }, { 0.577, 0.577, 0.577 } },
        { { 0, 1, 0 }, { 0, 1, 0, 1 }, { 0.577, 0.577, 0.577 } },
    };

    const Geometry<PCNVertex>::elem_array_type elems = { 0, 1, 2 };

    TEST_F(GeometryTest, RealGeometrySetVertexDataTest) {
        // Copy from refs.
        Geometry<PCNVertex> g;
        g.setVertexData(elems, verts);
        assertNoBuffersCreated(dynamic_cast<AbstractGeometry&>(g));
        ASSERT_EQ(3, g.getVertices().size());
        ASSERT_EQ(3, g.getElems().size());
        ASSERT_NE(&verts, &g.getVertices());
        ASSERT_NE(&elems, &g.getElems());

        // Move from refs.
        Geometry<PCNVertex> g2;
        Geometry<PCNVertex>::vertex_array_type new_verts = verts;
        Geometry<PCNVertex>::elem_array_type new_elems = elems;
        g2.setVertexData(std::move(new_elems), std::move(new_verts));
        assertNoBuffersCreated(dynamic_cast<AbstractGeometry&>(g2));
        ASSERT_EQ(3, g2.getVertices().size());
        ASSERT_EQ(3, g2.getElems().size());
        ASSERT_NE(&new_verts, &g2.getVertices());
        ASSERT_NE(&new_elems, &g2.getElems());
        ASSERT_TRUE(new_verts.empty());
        ASSERT_TRUE(new_elems.empty());

        // Copy from pointers.
        Geometry<PCNVertex> g3;
        new_verts = verts;
        new_elems = elems;
        g3.setVertexData(new_elems.data(), 3, new_verts.data(), 3);
        assertNoBuffersCreated(dynamic_cast<AbstractGeometry&>(g3));
        ASSERT_EQ(3, g3.getVertices().size());
        ASSERT_EQ(3, g3.getElems().size());
        ASSERT_NE(&new_verts, &g3.getVertices());
        ASSERT_NE(&new_elems, &g3.getElems());
        ASSERT_EQ(3, new_verts.size());
        ASSERT_EQ(3, new_elems.size());
    }
};
