// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_SPHERE_BODY_H_
#define _GRAPHPLAY_GRAPHPLAY_SPHERE_BODY_H_

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "BasicShader.h"
#include "Body.h"
#include "Mesh.h"

namespace graphplay {
    class SphereBody : public Body
    {
    public:
        SphereBody(BasicShader &shader);
        ~SphereBody();

        void render(const glm::mat4 &projection, const glm::mat4 &model_view, int flags);

    private:
        BasicShader &m_shader;
        GLuint m_color_buffer;
        Mesh *m_mesh;
    };
};

#endif
