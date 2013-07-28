// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "BasicShader.h"
#include "Mesh.h"
#include "SphereBody.h"

namespace graphplay {
    SphereBody::SphereBody(BasicShader &shader)
        : m_shader(shader),
          m_color_buffer(0),
          m_mesh()
    {
        /*const AttrInfo *pos_info;
        float *color_buffer;
        const float *vsoup;

        m_mesh = loadDaeFile("octohedron.dae");
        m_mesh->initSoupGLBuffer();
        pos_info = m_mesh->getAttrInfo("VERTEX");

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

        glGenBuffers(1, &m_color_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_color_buffer);
        glBufferData(GL_ARRAY_BUFFER,
            m_mesh->getNumVerts()*4*sizeof(float),
            color_buffer,
            GL_STATIC_DRAW);

        delete [] color_buffer;*/
    }

    SphereBody::~SphereBody(void)
    {
        if (m_mesh) {
            delete m_mesh;
        }

        if (glIsBuffer(m_color_buffer)) {
            glDeleteBuffers(1, &m_color_buffer);
        }
    }

    void SphereBody::render(const glm::mat4 &wld_projection, const glm::mat4 &wld_model_view, int flags)
    {
        glm::mat4 model_view = baseModelView(wld_model_view);

        glUseProgram(m_shader.getProgram());

        m_mesh->setVertexAttribPointer(m_shader.getPositionLocation(), "VERTEX");
    
        GLuint color_loc = m_shader.getColorLocation();
        glEnableVertexAttribArray(color_loc);
        glBindBuffer(GL_ARRAY_BUFFER, m_color_buffer);
        glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, 0);

        glUniformMatrix4fv(m_shader.getProjectionLocation(), 1, GL_FALSE, glm::value_ptr(wld_projection));
        glUniformMatrix4fv(m_shader.getModelViewLocation(), 1, GL_FALSE, glm::value_ptr(model_view));

        glDrawArrays(GL_TRIANGLES, 0, m_mesh->getNumVerts());
    }
};