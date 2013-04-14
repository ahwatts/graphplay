#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "DaeFile.h"
#include "Mesh.h"
#include "SphereBody.h"

SphereBody::SphereBody(GLuint shader)
    : m_shader(shader),
      m_vertex_buffer(0),
      m_color_buffer(0),
      m_mesh()
{
    GLuint bufs[2];
    const AttrInfo *pos_info;
    float *color_buffer;
    const float *vsoup;

    m_mesh = loadDaeFile("octohedron.dae");
    pos_info = m_mesh->getAttrInfo("VERTEX");

    glGenBuffers(2, bufs);
    m_vertex_buffer = bufs[0];
    m_color_buffer = bufs[1];

    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER,
        m_mesh->getSoupSizeInBytes(),
        m_mesh->getSoup(),
        GL_STATIC_DRAW);

    color_buffer = new float[m_mesh->getNumVerts()*4];
    vsoup = m_mesh->getSoup();

    for (int i = 0; i < m_mesh->getNumVerts(); ++i) {
        int cbuf_offset = 4*i;
        int vbuf_offset = m_mesh->getValsPerVert()*i + pos_info->offset;

        color_buffer[cbuf_offset+0] = abs(vsoup[vbuf_offset+0]);
        color_buffer[cbuf_offset+1] = abs(vsoup[vbuf_offset+1]);
        color_buffer[cbuf_offset+2] = abs(vsoup[vbuf_offset+2]);
        color_buffer[cbuf_offset+3] = 1.0f;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_color_buffer);
    glBufferData(GL_ARRAY_BUFFER,
        m_mesh->getNumVerts()*4*sizeof(float),
        color_buffer,
        GL_STATIC_DRAW);

    delete [] color_buffer;
}

SphereBody::~SphereBody(void)
{
    if (m_mesh) {
        delete m_mesh;
    }
}

void SphereBody::render(const glm::mat4x4 &wld_projection, const glm::mat4x4 &wld_model_view, int flags)
{
    glm::mat4x4 model_view = glm::translate(wld_model_view, mw_pos);
    model_view = glm::rotate(model_view, m_ang_pos, mw_ang_vel_dir);

    const AttrInfo *pos_info = m_mesh->getAttrInfo("VERTEX");

    glUseProgram(m_shader);

    GLuint position_loc = glGetAttribLocation(m_shader, "aPosition");
    glEnableVertexAttribArray(position_loc);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glVertexAttribPointer(position_loc, pos_info->width, GL_FLOAT, GL_FALSE,
        m_mesh->getValsPerVert()*sizeof(float),
        (const GLvoid*)(pos_info->offset*sizeof(float)));

    GLuint color_loc = glGetAttribLocation(m_shader, "aColor");
    glEnableVertexAttribArray(color_loc);
    glBindBuffer(GL_ARRAY_BUFFER, m_color_buffer);
    glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, 0);

    GLuint proj_loc = glGetUniformLocation(m_shader, "uProjection");
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(wld_projection));

    GLuint mv_loc = glGetUniformLocation(m_shader, "uModelView");
    glUniformMatrix4fv(mv_loc, 1, GL_FALSE, glm::value_ptr(model_view));

    glDrawArrays(GL_TRIANGLES, 0, m_mesh->getNumVerts());
}
