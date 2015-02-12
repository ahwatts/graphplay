// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_SHADER_H_
#define _GRAPHPLAY_GRAPHPLAY_SHADER_H_

#include "graphplay.h"

#include <map>
#include <memory>
#include <string>

#include "opengl.h"
#include "OpenGLUtils.h"

#include <glm/mat4x4.hpp>

namespace graphplay {
    struct ViewAndProjectionBlock {
        glm::mat4x4 view;
        glm::mat4x4 view_inv;
        glm::mat4x4 projection;
    };

    class Shader {
    public:
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

        inline const IndexMap& getAttributes()    const { return m_attributes; }
        inline const IndexMap& getUniforms()      const { return m_uniforms; }
        inline const IndexMap& getUniformBlocks() const { return m_uniform_blocks; };

        static const char *unlit_vertex_shader_source, *unlit_fragment_shader_source;
        static const char *lit_vertex_shader_source, *lit_fragment_shader_source;

    private:
        GLuint m_program;
        IndexMap m_attributes;
        IndexMap m_uniforms;
        IndexMap m_uniform_blocks;
    };
};

#endif
