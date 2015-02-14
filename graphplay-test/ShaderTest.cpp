// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "../graphplay/graphplay.h"

#include <gtest/gtest.h>

#include "TestOpenGLContext.h"
#include "../graphplay/Shader.h"

namespace graphplay {
    class ShaderTest : public TestOpenGLContext {};

    const char *vertex_shader_source = R"glsl(
        #version 410 core
        in vec3 position;
        uniform mat4x4 model;
        uniform view_and_projection {
            mat4x4 view;
            mat4x4 projection;
        };
        out vec4 v_color;
        void main(void) {
            gl_Position = projection * view * model * vec4(position, 1.0);
            v_color = vec4(clamp(abs(position), 0.0, 1.0), 1.0);
        }
    )glsl";

    const char *fragment_shader_source = R"glsl(
        #version 410 core
        in vec4 v_color;
        out vec4 FragColor;
        void main(void) {
            FragColor = v_color;
        }
    )glsl";

    TEST_F(ShaderTest, ShaderConstructor) {
        Shader s(GL_VERTEX_SHADER, vertex_shader_source);
        ASSERT_EQ(GL_TRUE, glIsShader(s.getShaderId()));
    }

    TEST_F(ShaderTest, ShaderDestructor) {
        GLuint shader_id = 0;

        {
            Shader s(GL_VERTEX_SHADER, vertex_shader_source);
            shader_id = s.getShaderId();
            ASSERT_EQ(GL_TRUE, glIsShader(shader_id));
        }

        ASSERT_EQ(GL_FALSE, glIsShader(shader_id));
    }

    TEST_F(ShaderTest, ProgramConstructor) {
        Shader::sptr_type v = std::make_shared<Shader>(GL_VERTEX_SHADER, vertex_shader_source);
        Shader::sptr_type f = std::make_shared<Shader>(GL_FRAGMENT_SHADER, fragment_shader_source);
        Program::sptr_type p = std::make_shared<Program>(v, f);

        ASSERT_EQ(GL_TRUE, glIsProgram(p->getProgramId()));
        ASSERT_EQ(v, p->getVertexShader());
        ASSERT_EQ(f, p->getFragmentShader());

        const IndexMap &attrs = p->getAttributes();
        auto index = attrs.find("position");
        ASSERT_NE(attrs.end(), index);

        const IndexMap &unifs = p->getUniforms();
        index = unifs.find("model");
        ASSERT_NE(unifs.end(), index);
        index = unifs.find("view");
        ASSERT_NE(unifs.end(), index);
        index = unifs.find("projection");
        ASSERT_NE(unifs.end(), index);

        const IndexMap &unifbs = p->getUniformBlocks();
        index = unifbs.find("view_and_projection");
        ASSERT_NE(unifbs.end(), index);
    }
}
