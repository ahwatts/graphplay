// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "../graphplay.h"
#include "Shader.h"

#include <iostream>
#include <regex>
#include <string>

namespace graphplay {
    // Global functions.
    Program::sptr_type createUnlitProgram() {
        Shader::sptr_type vertex = std::make_shared<Shader>(GL_VERTEX_SHADER, Shader::unlit_vertex_shader_source);
        Shader::sptr_type fragment = std::make_shared<Shader>(GL_FRAGMENT_SHADER, Shader::unlit_fragment_shader_source);
        return std::make_shared<Program>(vertex, fragment);
    }

    Program::sptr_type createLitProgram() {
        Shader::sptr_type vertex = std::make_shared<Shader>(GL_VERTEX_SHADER, Shader::lit_vertex_shader_source);
        Shader::sptr_type fragment = std::make_shared<Shader>(GL_FRAGMENT_SHADER, Shader::lit_fragment_shader_source);
        return std::make_shared<Program>(vertex, fragment);
    }

    // ViewAndProjectionBlock class.
    ViewAndProjectionBlock::ViewAndProjectionBlock()
        : m_offsets(),
          m_data_size(0)
    {
        Shader::sptr_type vert = std::make_shared<Shader>(GL_VERTEX_SHADER, Shader::lit_vertex_shader_source);
        Shader::sptr_type frag = std::make_shared<Shader>(GL_FRAGMENT_SHADER, Shader::lit_fragment_shader_source);
        Program::sptr_type prog = std::make_shared<Program>(vert, frag);

        GLuint progid = prog->getProgramId();
        GLuint vp_block_index = glGetUniformBlockIndex(progid, "view_and_projection");
        GLint vp_data_size = -1, num_uniforms = -1, *uniform_indices_int = nullptr, *uniform_offsets = nullptr, max_name_len = -1;
        GLuint *uniform_indices = nullptr;
        char *name = nullptr;

        glGetProgramiv(progid, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);
        name = new char[max_name_len];

        glGetActiveUniformBlockiv(progid, vp_block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &vp_data_size);
        m_data_size = vp_data_size;

        glGetActiveUniformBlockiv(progid, vp_block_index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &num_uniforms);
        uniform_indices_int = new GLint[num_uniforms];
        uniform_offsets = new GLint[num_uniforms];
        uniform_indices = new GLuint[num_uniforms];

        // Oy. Why do I have to get the list of indices as a GLint but pass it as a GLuint?
        glGetActiveUniformBlockiv(progid, vp_block_index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, uniform_indices_int);
        for (auto i = 0; i < num_uniforms; ++i) { uniform_indices[i] = uniform_indices_int[i]; }
        glGetActiveUniformsiv(progid, num_uniforms, uniform_indices, GL_UNIFORM_OFFSET, uniform_offsets);

        for (auto i = 0; i < num_uniforms; ++i) {
            glGetActiveUniformName(progid, uniform_indices[i], max_name_len, nullptr, name);
            std::string name_str(name);

            if (name_str == "view") {
                m_offsets[field_name::VIEW] = uniform_offsets[i];
            } else if (name_str == "view_inv") {
                m_offsets[field_name::VIEW_INV] = uniform_offsets[i];
            } else if (name_str == "projection") {
                m_offsets[field_name::PROJECTION] = uniform_offsets[i];
            }
        }

        delete [] name;
        delete [] uniform_indices_int;
        delete [] uniform_indices;
        delete [] uniform_offsets;
    }

    const ViewAndProjectionBlock& ViewAndProjectionBlock::getDescriptor() {
        static std::unique_ptr<ViewAndProjectionBlock> instance_ptr;

        // We don't want to do this at static initialization time,
        // because the OpenGL context probably won't be created yet.
        if (!instance_ptr) {
            instance_ptr.reset(new ViewAndProjectionBlock());
        }

        return *instance_ptr;
    }

    const ViewAndProjectionBlock::offset_map_type& ViewAndProjectionBlock::getOffsets() const {
        return m_offsets;
    }

    long ViewAndProjectionBlock::getDataSize() const {
        return m_data_size;
    }

    // LightListBlock class.
    LightListBlock::LightListBlock()
        : m_offsets(MAX_LIGHTS),
          m_data_size(0)
    {
        Shader::sptr_type vert = std::make_shared<Shader>(GL_VERTEX_SHADER, Shader::lit_vertex_shader_source);
        Shader::sptr_type frag = std::make_shared<Shader>(GL_FRAGMENT_SHADER, Shader::lit_fragment_shader_source);
        Program::sptr_type prog = std::make_shared<Program>(vert, frag);

        GLuint progid = prog->getProgramId();
        GLuint light_block_index = glGetUniformBlockIndex(progid, "light_list");
        GLint light_data_size = -1, num_uniforms = -1, *uniform_indices_int = nullptr, *uniform_offsets = nullptr, max_name_len = -1;
        GLuint *uniform_indices = nullptr;
        char *name = nullptr;
        std::regex light_field_name("^lights\\[(\\d+)\\]\\.(\\w+)$");
        std::cmatch light_field_parts;

        glGetProgramiv(progid, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);
        name = new char[max_name_len];

        glGetActiveUniformBlockiv(progid, light_block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &light_data_size);
        m_data_size = light_data_size;

        glGetActiveUniformBlockiv(progid, light_block_index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &num_uniforms);
        uniform_indices_int = new GLint[num_uniforms];
        uniform_offsets = new GLint[num_uniforms];
        uniform_indices = new GLuint[num_uniforms];

        // Oy. Why do I have to get the list of indices as a GLint but pass it as a GLuint?
        glGetActiveUniformBlockiv(progid, light_block_index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, uniform_indices_int);
        for (auto i = 0; i < num_uniforms; ++i) { uniform_indices[i] = uniform_indices_int[i]; }
        glGetActiveUniformsiv(progid, num_uniforms, uniform_indices, GL_UNIFORM_OFFSET, uniform_offsets);

        for (auto i = 0; i < num_uniforms; ++i) {
            glGetActiveUniformName(progid, uniform_indices[i], max_name_len, nullptr, name);
            if (std::regex_search(name, light_field_parts, light_field_name)) {
                int light_index = std::stoi(light_field_parts[1]);
                std::string field_str(light_field_parts[2]);

                if (field_str == "enabled") {
                    m_offsets[light_index][field_name::ENABLED] = uniform_offsets[i];
                } else if (field_str == "position") {
                    m_offsets[light_index][field_name::POSITION] = uniform_offsets[i];
                } else if (field_str == "color") {
                    m_offsets[light_index][field_name::COLOR] = uniform_offsets[i];
                } else if (field_str == "specular_exp") {
                    m_offsets[light_index][field_name::SPECULAR_EXP] = uniform_offsets[i];
                }
            }
        }

        delete [] name;
        delete [] uniform_indices_int;
        delete [] uniform_indices;
        delete [] uniform_offsets;
    }

    const LightListBlock& LightListBlock::getDescriptor() {
        static std::unique_ptr<LightListBlock> instance_ptr;

        // We don't want to do this at static initialization time,
        // because the OpenGL context probably won't be created yet.
        if (!instance_ptr) {
            instance_ptr.reset(new LightListBlock());
        }

        return *instance_ptr;
    }

    const std::vector<LightListBlock::offset_map_type>& LightListBlock::getOffsets() const {
        return m_offsets;
    }

    long LightListBlock::getDataSize() const {
        return m_data_size;
    }

    // Shader class.
    Shader::Shader(GLenum type, const char *source) : m_shader{0} {
        m_shader = createAndCompileShader(type, source);
    }

    Shader::~Shader() {
        if (glIsShader(m_shader)) {
            glDeleteShader(m_shader);
        }
    }

    // Program class.
    Program::Program(Shader::sptr_type vertex_shader, Shader::sptr_type fragment_shader)
        : m_program{0},
          m_vertex_shader{vertex_shader},
          m_fragment_shader{fragment_shader},
          m_attributes(),
          m_uniforms(),
          m_uniform_blocks()
    {
        m_program = createProgramFromShaders(
            m_vertex_shader->getShaderId(),
            m_fragment_shader->getShaderId());
        getAttributeInfo(m_program, m_attributes);
        getUniformInfo(m_program, m_uniforms);
        getUniformBlockInfo(m_program, m_uniform_blocks);
    }

    Program::Program(const Program &other)
        : m_program{0},
          m_vertex_shader{other.m_vertex_shader},
          m_fragment_shader{other.m_fragment_shader},
          m_attributes(),
          m_uniforms(),
          m_uniform_blocks()
    {
        m_program = createProgramFromShaders(
            m_vertex_shader->getShaderId(),
            m_fragment_shader->getShaderId());
        getAttributeInfo(m_program, m_attributes);
        getUniformInfo(m_program, m_uniforms);
        getUniformBlockInfo(m_program, m_uniform_blocks);
    }

    Program::Program(Program &&other)
        : m_program{other.m_program},
          m_vertex_shader{other.m_vertex_shader},
          m_fragment_shader{other.m_fragment_shader},
          m_attributes(),
          m_uniforms(),
          m_uniform_blocks()
    {
        other.m_program = 0;
        std::swap(m_attributes, other.m_attributes);
        std::swap(m_uniforms, other.m_uniforms);
        std::swap(m_uniform_blocks, other.m_uniform_blocks);
    }

    Program::~Program() {
        if (glIsProgram(m_program)) {
            std::vector<GLuint> shaders;
            getAttachedShaders(m_program, shaders);

            for (auto s : shaders) {
                if (glIsShader(s)) {
                    // The shader *should* get deleted when its
                    // shared pointers go out of scope...
                    glDetachShader(m_program, s);
                }
            }

            glDeleteProgram(m_program);
            m_program = 0;
        }
    }

    Program& Program::operator=(const Program &other) {
        Program tmp(other);
        std::swap(*this, tmp);
        return *this;
    }

    Program& Program::operator=(Program &&other) {
        std::swap(m_program, other.m_program);
        std::swap(m_vertex_shader, other.m_vertex_shader);
        std::swap(m_fragment_shader, other.m_fragment_shader);
        std::swap(m_attributes, other.m_attributes);
        std::swap(m_uniforms, other.m_uniforms);
        std::swap(m_uniform_blocks, other.m_uniform_blocks);
        return *this;
    }

    // Actual shader code.
    const char *Shader::unlit_vertex_shader_source = R"glsl(
        #version 410 core

        in vec3 position;
        in vec4 color;

        uniform mat4x4 model;
        uniform mat3x3 model_inv_trans_3;
        layout (shared) uniform view_and_projection {
            mat4x4 view;
            mat4x4 view_inv;
            mat4x4 projection;
        };

        out vec4 v_color;

        void main(void) {
            gl_Position = projection * view * model * vec4(position, 1.0);
            v_color = color;
        }
    )glsl";

    const char *Shader::unlit_fragment_shader_source = R"glsl(
        #version 410 core

        in vec4 v_color;

        out vec4 FragColor;

        void main(void) {
            FragColor = v_color;
        }
    )glsl";

    const char *Shader::lit_vertex_shader_source = R"glsl(
        #version 410 core

        const int MAX_LIGHTS = 10;
        struct LightProperties {
            bool enabled;
            vec3 position;
            vec4 color;
            uint specular_exp;
        };

        in vec3 position;
        in vec3 normal;
        in vec4 color;

        uniform mat4x4 model;
        uniform mat3x3 model_inv_trans_3;
        layout (shared) uniform view_and_projection {
            mat4x4 view;
            mat4x4 view_inv;
            mat4x4 projection;
        };
        layout (shared) uniform light_list {
            LightProperties lights[MAX_LIGHTS];
        };

        out vec3 v_normal;
        out vec4 v_color;
        out vec3 v_eye_dir;
        out vec3 v_light_dir[MAX_LIGHTS];
        out vec3 v_light_reflect_dir[MAX_LIGHTS];

        void main(void) {
            vec4 wld_vert_position4 = model * vec4(position, 1.0);
            vec3 wld_vert_position = wld_vert_position4.xyz / wld_vert_position4.w;

            vec4 wld_eye_position4 = view_inv * vec4(0.0, 0.0, 0.0, 1.0);
            vec3 wld_eye_position = wld_eye_position4.xyz / wld_eye_position4.w;

            vec3 wld_vert_normal = normalize(model_inv_trans_3 * normal);

            vec3 wld_vert_eye_dir = normalize(wld_eye_position - wld_vert_position);

            gl_Position = projection * view * wld_vert_position4;
            v_color = color;
            v_eye_dir = wld_vert_eye_dir;
            v_normal = wld_vert_normal;
            for (int i = 0; i < MAX_LIGHTS; ++i) {
                if (lights[i].enabled) {
                    v_light_dir[i] = normalize(lights[i].position - wld_vert_position);
                    v_light_reflect_dir[i] = normalize(reflect(-1 * v_light_dir[i], wld_vert_normal));
                }
            }
        }
    )glsl";

    const char *Shader::lit_fragment_shader_source = R"glsl(
        #version 410 core

        const int MAX_LIGHTS = 10;
        struct LightProperties {
            bool enabled;
            vec3 position;
            vec4 color;
            uint specular_exp;
        };

        in vec3 v_normal;
        in vec4 v_color;
        in vec3 v_eye_dir;
        in vec3 v_light_dir[MAX_LIGHTS];
        in vec3 v_light_reflect_dir[MAX_LIGHTS];

        layout (shared) uniform light_list {
            LightProperties lights[MAX_LIGHTS];
        };

        out vec4 frag_color;

        void main(void) {
            frag_color = vec4(0.0, 0.0, 0.0, 1.0);

            for (int i = 0; i < MAX_LIGHTS; ++i) {
                if (lights[i].enabled) {
                    vec3 color_combination = lights[i].color.rgb * v_color.rgb;

                    vec3 ambient_color = 0.1 * color_combination;

                    float diffuse_coeff = 0.7 * max(0.0, dot(v_normal, v_light_dir[i]));
                    vec3 diffuse_color = diffuse_coeff * color_combination;

                    vec3 specular_color = vec3(0.0, 0.0, 0.0);
                    if (dot(v_normal, v_light_dir[i]) >= 0.0) {
                        float spec_coeff = 0.7 * pow(max(0.0, dot(v_light_reflect_dir[i], v_eye_dir)), lights[i].specular_exp);
                        specular_color = spec_coeff * color_combination;
                    }

                    frag_color = clamp(frag_color + vec4(clamp(ambient_color + diffuse_color + specular_color, 0.0, 1.0), v_color.a), 0.0, 1.0);
                }
            }
        }
    )glsl";
}
