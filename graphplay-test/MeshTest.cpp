// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <memory>
#include <gtest/gtest.h>

#include "../graphplay/Geometry.h"
#include "../graphplay/Material.h"
#include "../graphplay/Mesh.h"
#include "TestOpenGLContext.h"

namespace graphplay {
    class MeshTest : public TestOpenGLContext { };

    TEST_F(MeshTest, DefaultConstructor) {
        Mesh m;

        wp_Geometry geo = m.getGeometry();
        sp_Geometry sgeo = geo.lock();
        ASSERT_EQ(nullptr, sgeo);

        wp_Material mat = m.getMaterial();
        sp_Material smat = mat.lock();
        ASSERT_EQ(nullptr, smat);
    }

    TEST_F(MeshTest, SetGeometry) {
        Mesh m;
        sp_Geometry geo(new OctohedronGeometry());
        m.setGeometry(geo);
        ASSERT_EQ(geo, m.getGeometry().lock());
    }

    TEST_F(MeshTest, SetMaterial) {
        Mesh m;
        sp_Material mat(new LambertMaterial());
        m.setMaterial(mat);
        ASSERT_EQ(mat, m.getMaterial().lock());
    }
};
