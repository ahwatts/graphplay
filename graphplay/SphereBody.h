#ifndef _SPHERE_BODY_H_
#define _SPHERE_BODY_H_

#include <GL/glew.h>

#include "Body.h"

class SphereBody : public Body
{
public:
    SphereBody(GLuint shader);
    ~SphereBody();

    GLuint m_shader;
    GLuint m_vertex_buffer;
    GLuint m_color_buffer;

    void render(int flags);
};

#endif
