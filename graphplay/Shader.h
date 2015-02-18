// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_SHADER_H_
#define _GRAPHPLAY_GRAPHPLAY_SHADER_H_

#include <memory>
#include "OpenGLUtils.h"

namespace graphplay {
    constexpr unsigned int MAX_LIGHTS = 10;

    struct ViewAndProjectionBlock {
        GLfloat view[16];
        GLfloat view_inv[16];
        GLfloat projection[16];
    };

    struct LightPropertiesBlock {
        GLuint enabled;
        GLfloat position[3];
        GLfloat color[4];
        GLuint specular_exp;
    };

    struct LightListBlock {
        LightPropertiesBlock lights[MAX_LIGHTS];
    };

    class Shader {
    public:
        typedef std::unique_ptr<Shader> uptr_type;
        typedef std::shared_ptr<Shader> sptr_type;
        typedef std::weak_ptr<Shader> wptr_type;

        Shader(GLenum type, const char *source);
        Shader(const Shader &other) = delete;
        Shader(Shader &&other) = delete;
        ~Shader();

        Shader& operator=(Shader &other) = delete;
        Shader& operator=(Shader &&other) = delete;

        inline const GLuint getShaderId() const { return m_shader; }

        static const char *unlit_vertex_shader_source, *unlit_fragment_shader_source;
        static const char *lit_vertex_shader_source, *lit_fragment_shader_source;

    private:
        GLuint m_shader;
    };

    class Program {
    public:
        typedef std::unique_ptr<Program> uptr_type;
        typedef std::shared_ptr<Program> sptr_type;
        typedef std::weak_ptr<Program> wptr_type;

        Program(Shader::sptr_type vertex_shader, Shader::sptr_type fragment_shader);
        Program(const Program &other);
        Program(Program &&other);
        ~Program();

        Program& operator=(const Program &other);
        Program& operator=(Program &&other);

        inline const GLuint getProgramId() const { return m_program; }
        inline const Shader::sptr_type getVertexShader()   const { return m_vertex_shader; }
        inline const Shader::sptr_type getFragmentShader() const { return m_fragment_shader; }
        inline const IndexMap& getAttributes()    const { return m_attributes; }
        inline const IndexMap& getUniforms()      const { return m_uniforms; }
        inline const IndexMap& getUniformBlocks() const { return m_uniform_blocks; };

    private:
        GLuint m_program;
        Shader::sptr_type m_vertex_shader, m_fragment_shader;
        IndexMap m_attributes, m_uniforms, m_uniform_blocks;
    };
};

#endif
