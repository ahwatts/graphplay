#ifndef _SPHERE_BODY_H_
#define _SPHERE_BODY_H_

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Body.h"
#include "Mesh.h"

class SphereBody : public Body
{
public:
    SphereBody(GLuint shader);
    ~SphereBody();

    void render(const glm::mat4x4 &projection, const glm::mat4x4 &model_view, int flags);

private:
    GLuint m_shader;
    GLuint m_vertex_buffer;
    GLuint m_color_buffer;
    Mesh *m_mesh;
};

#endif
