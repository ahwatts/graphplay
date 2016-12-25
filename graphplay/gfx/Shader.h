// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_GFX_SHADER_H_
#define _GRAPHPLAY_GRAPHPLAY_GFX_SHADER_H_

#include "../graphplay.h"

#include <map>
#include <memory>
#include <vector>
#include "OpenGLUtils.h"

namespace graphplay {
    class ViewAndProjectionBlock {
    public:
        enum field_name {
            VIEW,
            VIEW_INV,
            PROJECTION,
        };
        typedef std::map<field_name, GLuint> offset_map_type;

        static const ViewAndProjectionBlock& getDescriptor();

        const offset_map_type& getOffsets() const;
        long getDataSize() const;

    private:
        ViewAndProjectionBlock();
        ViewAndProjectionBlock(const ViewAndProjectionBlock &other) = delete;
        ViewAndProjectionBlock(ViewAndProjectionBlock &&other) = delete;
        ViewAndProjectionBlock& operator=(const ViewAndProjectionBlock &other) = delete;
        ViewAndProjectionBlock& operator=(ViewAndProjectionBlock &&other) = delete;

        offset_map_type m_offsets;
        long m_data_size;
    };

    class LightListBlock {
    public:
        static const int MAX_LIGHTS = 10;

        enum field_name {
            ENABLED,
            POSITION,
            COLOR,
            SPECULAR_EXP,
        };
        typedef std::map<field_name, GLuint> offset_map_type;

        static const LightListBlock& getDescriptor();

        const std::vector<offset_map_type>& getOffsets() const;
        long getDataSize() const;

    private:
        LightListBlock();
        LightListBlock(const LightListBlock &other) = delete;
        LightListBlock(LightListBlock &&other) = delete;
        LightListBlock& operator=(const LightListBlock &other) = delete;
        LightListBlock& operator=(LightListBlock &&other) = delete;

        std::vector<offset_map_type> m_offsets;
        long m_data_size;
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

    Program::sptr_type createUnlitProgram();
    Program::sptr_type createLitProgram();
};

#endif
