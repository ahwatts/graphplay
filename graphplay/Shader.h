// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_SHADER_H_
#define _GRAPHPLAY_GRAPHPLAY_SHADER_H_

#include <map>
#include <string>

#include "opengl.h"

namespace graphplay {
    class Shader {
    public:
        Shader(const char *vertex_shader_source, const char *fragment_shader_source);
        ~Shader();

        void dump() const;

        typedef std::map<std::string, GLuint> index_map;

        index_map::const_iterator getAttributes() const;
        index_map::const_iterator getUniforms() const;
        index_map::const_iterator getUniformBlocks() const;

    private:
        GLuint m_program;
        index_map m_attributes;
        index_map m_uniforms;
        index_map m_uniform_blocks;
    };
};

#endif