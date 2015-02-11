// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_SHADER_H_
#define _GRAPHPLAY_GRAPHPLAY_SHADER_H_

#include <map>
#include <memory>
#include <string>

#include "opengl.h"

namespace graphplay {
    class Shader {
    public:
        typedef std::map<std::string, GLuint> index_map_type;
        typedef std::unique_ptr<Shader> uptr_type;
        typedef std::shared_ptr<Shader> sptr_type;
        typedef std::weak_ptr<Shader> wptr_type;

        Shader(const char *vertex_shader_source, const char *fragment_shader_source);
        Shader(const Shader &other);
        Shader(Shader &&other);
        ~Shader();

        Shader& operator=(Shader &other);
        Shader& operator=(Shader &&other);

        void dump() const;

        inline const index_map_type& getAttributes()    const { return m_attributes; }
        inline const index_map_type& getUniforms()      const { return m_uniforms; }
        inline const index_map_type& getUniformBlocks() const { return m_uniform_blocks; };

        static const char *unlit_vertex_shader_source, *unlit_fragment_shader_source;
        static const char *lit_vertex_shader_source, *lit_fragment_shader_source;

    private:
        GLuint m_program;
        index_map_type m_attributes;
        index_map_type m_uniforms;
        index_map_type m_uniform_blocks;
    };
};

#endif
