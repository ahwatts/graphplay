// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "../graphplay/graphplay.h"

#include <gtest/gtest.h>

#include "TestOpenGLContext.h"
#include "../graphplay/Geometry.h"

namespace graphplay {
    class GeometryTest : public TestOpenGLContext {};

    void assertNoBuffersCreated(const AbstractGeometry &g) {
        ASSERT_EQ(0, g.elemBufferId());
        ASSERT_EQ(GL_FALSE, glIsBuffer(g.elemBufferId()));
        ASSERT_EQ(GL_NO_ERROR, glGetError());

        ASSERT_EQ(0, g.vertexBufferId());
        ASSERT_EQ(GL_FALSE, glIsBuffer(g.vertexBufferId()));
        ASSERT_EQ(GL_NO_ERROR, glGetError());

        ASSERT_EQ(0, g.vertexArrayObjectId());
        ASSERT_EQ(GL_FALSE, glIsVertexArray(g.vertexArrayObjectId()));
        ASSERT_EQ(GL_NO_ERROR, glGetError());
    }

    void assertBuffersCreated(const AbstractGeometry &g) {
        ASSERT_EQ(GL_TRUE, glIsBuffer(g.elemBufferId()));
        ASSERT_EQ(GL_TRUE, glIsBuffer(g.vertexBufferId()));
        ASSERT_EQ(0, g.vertexArrayObjectId());
        ASSERT_EQ(GL_FALSE, glIsVertexArray(g.vertexArrayObjectId()));
    }

    void assertEqualBufferContent(GLuint expected, GLuint actual) {
        ASSERT_EQ(GL_TRUE, glIsBuffer(expected));
        ASSERT_EQ(GL_TRUE, glIsBuffer(actual));

        int ex_size = 0;
        glBindBuffer(GL_ARRAY_BUFFER, expected);
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &ex_size);
        char *ex_buf_ptr = (char *)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
        std::vector<char> ex_buf(ex_buf_ptr, ex_buf_ptr + ex_size);
        glUnmapBuffer(GL_ARRAY_BUFFER);

        int act_size = 0;
        glBindBuffer(GL_ARRAY_BUFFER, actual);
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &act_size);
        char *act_buf_ptr = (char *)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
        std::vector<char> act_buf(act_buf_ptr, act_buf_ptr + act_size);
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        ASSERT_EQ(ex_size, act_size);
        auto ei = ex_buf.cbegin(), ai = act_buf.cbegin();
        while (ei != ex_buf.cend() && ai != act_buf.cend()) {
            ASSERT_EQ(*ei, *ai);
            ++ei; ++ai;
        }
        ASSERT_EQ(ex_buf.cend(), ei);
        ASSERT_EQ(act_buf.cend(), ai);
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
        ASSERT_TRUE(g.vertices().empty());
        ASSERT_TRUE(g.elements().empty());
        ASSERT_EQ(&PCNVertex::description, &g.attrInfos());
    }

    const Geometry<PCNVertex>::vertex_array_type verts = {
        { { 0, 0, 1 }, { 0, 0, 1, 1 }, { 0.577f, 0.577f, 0.577f } },
        { { 1, 0, 0 }, { 1, 0, 0, 1 }, { 0.577f, 0.577f, 0.577f } },
        { { 0, 1, 0 }, { 0, 1, 0, 1 }, { 0.577f, 0.577f, 0.577f } },
    };
    const Geometry<PCNVertex>::elem_array_type elems = { 0, 1, 2 };

    TEST_F(GeometryTest, RealGeometrySetVertexDataTest) {
        // Copy from refs.
        Geometry<PCNVertex> g;
        g.setVertexData(elems, verts);
        assertNoBuffersCreated(dynamic_cast<AbstractGeometry&>(g));
        ASSERT_EQ(3, g.vertices().size());
        ASSERT_EQ(3, g.elements().size());
        ASSERT_NE(&verts, &g.vertices());
        ASSERT_NE(&elems, &g.elements());

        // Move from refs.
        Geometry<PCNVertex> g2;
        Geometry<PCNVertex>::vertex_array_type new_verts = verts;
        Geometry<PCNVertex>::elem_array_type new_elems = elems;
        g2.setVertexData(std::move(new_elems), std::move(new_verts));
        assertNoBuffersCreated(dynamic_cast<AbstractGeometry&>(g2));
        ASSERT_EQ(3, g2.vertices().size());
        ASSERT_EQ(3, g2.elements().size());
        ASSERT_NE(&new_verts, &g2.vertices());
        ASSERT_NE(&new_elems, &g2.elements());
        ASSERT_TRUE(new_verts.empty());
        ASSERT_TRUE(new_elems.empty());

        // Copy from pointers.
        Geometry<PCNVertex> g3;
        new_verts = verts;
        new_elems = elems;
        g3.setVertexData(new_elems.data(), 3, new_verts.data(), 3);
        assertNoBuffersCreated(dynamic_cast<AbstractGeometry&>(g3));
        ASSERT_EQ(3, g3.vertices().size());
        ASSERT_EQ(3, g3.elements().size());
        ASSERT_NE(&new_verts, &g3.vertices());
        ASSERT_NE(&new_elems, &g3.elements());
        ASSERT_EQ(3, new_verts.size());
        ASSERT_EQ(3, new_elems.size());
    }

    TEST_F(GeometryTest, RealGeometryCreateBuffers) {
        Geometry<PCNVertex> g;
        g.setVertexData(elems, verts);
        g.createBuffers();
        assertBuffersCreated(dynamic_cast<AbstractGeometry&>(g));
    }

    TEST_F(GeometryTest, RealGeometryDeleteBuffers) {
        Geometry<PCNVertex> g;
        g.setVertexData(elems, verts);
        g.createBuffers();

        GLuint elem_buffer = g.elemBufferId();
        GLuint vert_buffer = g.vertexBufferId();
        GLuint vert_array_obj = g.vertexArrayObjectId();

        assertBuffersCreated(dynamic_cast<AbstractGeometry&>(g));

        g.deleteBuffers();

        assertNoBuffersCreated(dynamic_cast<AbstractGeometry&>(g));

        ASSERT_EQ(GL_FALSE, glIsBuffer(elem_buffer));
        ASSERT_EQ(GL_FALSE, glIsBuffer(vert_buffer));
        ASSERT_EQ(GL_FALSE, glIsVertexArray(vert_array_obj));
    }

    TEST_F(GeometryTest, RealGeometryCopyConstructor) {
        Geometry<PCNVertex> g1;
        g1.setVertexData(elems, verts);
        g1.createBuffers();

        Geometry<PCNVertex> g2(g1);

        assertBuffersCreated(dynamic_cast<AbstractGeometry&>(g1));
        assertBuffersCreated(dynamic_cast<AbstractGeometry&>(g2));

        ASSERT_NE(g1.elemBufferId(), g2.elemBufferId());
        ASSERT_NE(g1.vertexBufferId(), g2.vertexBufferId());

        assertEqualBufferContent(g1.elemBufferId(), g2.elemBufferId());
        assertEqualBufferContent(g1.vertexBufferId(), g2.vertexBufferId());
    }

    TEST_F(GeometryTest, RealGeometryMoveConstructor) {
        Geometry<PCNVertex> g1;
        g1.setVertexData(elems, verts);
        g1.createBuffers();

        assertBuffersCreated(g1);

        Geometry<PCNVertex> g2(std::move(g1));

        assertNoBuffersCreated(g1);
        assertBuffersCreated(g2);
    }

    TEST_F(GeometryTest, RealGeometryCopyAssignmentOperator) {
        Geometry<PCNVertex> g1;
        g1.setVertexData(elems, verts);
        g1.createBuffers();

        Geometry<PCNVertex> g2;

        g2 = g1;

        assertBuffersCreated(dynamic_cast<AbstractGeometry&>(g1));
        assertBuffersCreated(dynamic_cast<AbstractGeometry&>(g2));

        ASSERT_NE(g1.elemBufferId(), g2.elemBufferId());
        ASSERT_NE(g1.vertexBufferId(), g2.vertexBufferId());

        assertEqualBufferContent(g1.elemBufferId(), g2.elemBufferId());
        assertEqualBufferContent(g1.vertexBufferId(), g2.vertexBufferId());
    }

    TEST_F(GeometryTest, RealGeometryMoveAssignmentOperator) {
        Geometry<PCNVertex> g1;
        g1.setVertexData(elems, verts);
        g1.createBuffers();

        assertBuffersCreated(g1);

        Geometry<PCNVertex> g2;

        g2 = std::move(g1);

        assertNoBuffersCreated(g1);
        assertBuffersCreated(g2);
    }

    TEST_F(GeometryTest, RealGeometryDestructor) {
        GLuint vbuf = 0, ebuf = 0;

        {
            Geometry<PCNVertex> g;
            g.setVertexData(elems, verts);
            g.createBuffers();
            vbuf = g.vertexBufferId();
            ebuf = g.elemBufferId();
            ASSERT_EQ(GL_TRUE, glIsBuffer(vbuf));
            ASSERT_EQ(GL_TRUE, glIsBuffer(ebuf));
        }

        ASSERT_EQ(GL_FALSE, glIsBuffer(vbuf));
        ASSERT_EQ(GL_FALSE, glIsBuffer(ebuf));
    }

    TEST_F(GeometryTest, CreateOctohedron) {
        Geometry<PCNVertex>::sptr_type octo = makeOctohedronGeometry();

        assertBuffersCreated(*octo);
        ASSERT_LT(0, static_cast<int>(octo->elements().size()));
        ASSERT_LT(0, static_cast<int>(octo->vertices().size()));
    }

    TEST_F(GeometryTest, CreateSphere) {
        Geometry<PCNVertex>::sptr_type sphere = makeSphereGeometry();
        assertBuffersCreated(*sphere);
        ASSERT_LT(0, static_cast<int>(sphere->elements().size()));
        ASSERT_LT(0, static_cast<int>(sphere->vertices().size()));
    }
};
