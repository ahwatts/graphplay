// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <gtest/gtest.h>

#include "TestOpenGLContext.h"
#include "../graphplay/Material.h"

namespace graphplay {
    class MaterialTest : public TestOpenGLContext { };

    TEST_F(MaterialTest, DefaultConstructor) {
        GouraudMaterial mat;

        ASSERT_FALSE(glIsProgram(mat.getProgram()));
        ASSERT_FALSE(glIsShader(mat.getVertexShader()));
        ASSERT_FALSE(glIsShader(mat.getFragmentShader()));
    }
};
