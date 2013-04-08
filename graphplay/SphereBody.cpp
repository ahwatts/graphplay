#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_access.hpp>
#include "SphereBody.h"

SphereBody::SphereBody(GLuint shader)
    : m_shader(shader),
      m_vertex_buffer(0),
      m_color_buffer(0)
{
    GLuint bufs[2];
    //glm::vec3 verts[24];
    //glm::vec4 colors[24];
    float verts[24*3];
    float colors[24*4];
    int i;

    //glm::set(verts[ 0],  0.0f, -1.0f,  0.0f);   glm::set(verts[ 3],  0.0f, -1.0f,  0.0f);
	//glm::set(verts[ 1],  1.0f,  0.0f,  0.0f);   glm::set(verts[ 4],  0.0f,  0.0f, -1.0f);
	//glm::set(verts[ 2],  0.0f,  0.0f,  1.0f);   glm::set(verts[ 5],  1.0f,  0.0f,  0.0f);

	//glm::set(verts[ 6],  0.0f, -1.0f,  0.0f);   glm::set(verts[ 9],  0.0f, -1.0f,  0.0f);
	//glm::set(verts[ 7], -1.0f,  0.0f,  0.0f);   glm::set(verts[10],  0.0f,  0.0f,  1.0f);
	//glm::set(verts[ 8],  0.0f,  0.0f, -1.0f);   glm::set(verts[11], -1.0f,  0.0f,  0.0f);

	//glm::set(verts[12],  0.0f,  1.0f,  0.0f);   glm::set(verts[15],  0.0f,  1.0f,  0.0f);
	//glm::set(verts[13],  0.0f,  0.0f,  1.0f);   glm::set(verts[16],  1.0f,  0.0f,  0.0f);
	//glm::set(verts[14],  1.0f,  0.0f,  0.0f);   glm::set(verts[17],  0.0f,  0.0f, -1.0f);

	//glm::set(verts[18],  0.0f,  1.0f,  0.0f);   glm::set(verts[21],  0.0f,  1.0f,  0.0f);
	//glm::set(verts[19],  0.0f,  0.0f, -1.0f);   glm::set(verts[22], -1.0f,  0.0f,  0.0f);
	//glm::set(verts[20], -1.0f,  0.0f,  0.0f);   glm::set(verts[23],  0.0f,  0.0f,  1.0f);

    //for (i = 0; i < 24; ++i) {
    //    glm::set(colors[i], abs(verts[i].x), abs(verts[i].y), abs(verts[i].z), 1.0f);
    //}

    glGenBuffers(2, bufs);
    m_vertex_buffer = bufs[0];
    m_color_buffer = bufs[1];

    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 24, glm::value_ptr(verts), GL_STATIC_DRAW);
}

SphereBody::~SphereBody(void)
{
}
