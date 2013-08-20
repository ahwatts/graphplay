// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <gtest/gtest.h>

#include "TestOpenGLContext.h"
#include "../graphplay/Material.h"

namespace graphplay {
    class MaterialTest : public TestOpenGLContext { };

    TEST_F(MaterialTest, DefaultConstructor) {
        GouraudMaterial mat;

        ASSERT_EQ(GL_FALSE, glIsProgram(mat.getProgram()));
        ASSERT_EQ(GL_FALSE, glIsShader(mat.getVertexShader()));
        ASSERT_EQ(GL_FALSE, glIsShader(mat.getFragmentShader()));

        ASSERT_GT(0, mat.getPositionLocation());
        ASSERT_GT(0, mat.getNormalLocation());
        ASSERT_GT(0, mat.getColorLocation());
        ASSERT_GT(0, mat.getTexCoordLocation());

        ASSERT_GT(0, mat.getProjectionLocation());
        ASSERT_GT(0, mat.getModelViewLocation());
    }

    TEST_F(MaterialTest, CreateProgram) {
        GouraudMaterial mat;

        mat.createProgram();
        ASSERT_EQ(GL_TRUE, glIsProgram(mat.getProgram()));
        ASSERT_EQ(GL_TRUE, glIsShader(mat.getVertexShader()));
        ASSERT_EQ(GL_TRUE, glIsShader(mat.getFragmentShader()));

        ASSERT_LE(0, mat.getPositionLocation());
        ASSERT_GT(0, mat.getNormalLocation());
        ASSERT_LE(0, mat.getColorLocation());
        ASSERT_GT(0, mat.getTexCoordLocation());

        ASSERT_LE(0, mat.getProjectionLocation());
        ASSERT_LE(0, mat.getModelViewLocation());
    }

    TEST_F(MaterialTest, MaterialLifecycle) {
        GouraudMaterial mat;

        mat.createProgram();
        mat.destroyProgram();

        ASSERT_EQ(GL_FALSE, glIsProgram(mat.getProgram()));
        ASSERT_EQ(GL_FALSE, glIsShader(mat.getVertexShader()));
        ASSERT_EQ(GL_FALSE, glIsShader(mat.getFragmentShader()));
    }

    TEST_F(MaterialTest, Destructor) {
        Material *mat = new GouraudMaterial();
        mat->createProgram();
        GLuint p = mat->getProgram();
        GLuint s1 = mat->getVertexShader();
        GLuint s2 = mat->getFragmentShader();
        delete mat;
        mat = nullptr;

        ASSERT_EQ(GL_FALSE, glIsProgram(p));
        ASSERT_EQ(GL_FALSE, glIsShader(s1));
        ASSERT_EQ(GL_FALSE, glIsShader(s2));
    }
};
