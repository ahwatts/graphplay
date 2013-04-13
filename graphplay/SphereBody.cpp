#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_access.hpp>
#include "DaeFile.h"
#include "Mesh.h"
#include "SphereBody.h"

SphereBody::SphereBody(GLuint shader)
    : m_shader(shader),
      m_vertex_buffer(0),
      m_color_buffer(0)
{
    //GLuint bufs[2];
    Mesh *octo_mesh = loadDaeFile("octohedron.dae");
    std::vector<AttrInfo> attrs;

    octo_mesh->getAttrInfo(attrs);

    /*glGenBuffers(2, bufs);
    m_vertex_buffer = bufs[0];
    m_color_buffer = bufs[1];

    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    //glBufferData(GL_ARRAY_BUFFER, 24, glm::value_ptr(verts), GL_STATIC_DRAW);*/

    delete octo_mesh;
}

SphereBody::~SphereBody(void)
{
}

void SphereBody::render(int flags) {

}
