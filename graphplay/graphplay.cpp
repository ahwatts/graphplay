// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

/*#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <libxml/xmlversion.h>
#include <libxml/parser.h>

#include "BasicShader.h"
#include "Collada.h"
#include "DaeFile.h"
#include "Mesh.h"
#include "World.h"
#include "graphplay.h"*/

#include <cstdio>
#include "Collada.h"

/*BasicShader *g_shader = NULL;
World *g_world = NULL;

void display();
void keypress(unsigned char key, int x, int y);
void reshape(int new_width, int new_height);
void update(int dt);*/

int main(int argc, char **argv)
{
    /*GLenum glew_err;
    int width = 800, height = 600, dt_msec = 20;

    LIBXML_TEST_VERSION;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    glutCreateWindow("graphplay");

    glew_err = glewInit();
    if (glew_err != GLEW_OK) {
        fprintf(stderr, "Could not initialize GLEW: %s\n", glewGetErrorString(glew_err));
        exit(1);
    }

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);

    g_shader = new BasicShader();
    g_world = new World(width, height);

    glutDisplayFunc(display);
    glutKeyboardFunc(keypress);
    glutReshapeFunc(reshape);
    glutTimerFunc(dt_msec, update, dt_msec);
    glutMainLoop();

    delete g_shader;
    delete g_world;
    xmlCleanupParser();
    xmlMemoryDump();*/

    unsigned int i;
    std::map<std::string, collada::Source>::const_iterator j;
    std::map<std::string, collada::SharedInput>::const_iterator k;

    std::vector<collada::MeshGeometry> geos;
    collada::loadGeometriesFromFile(geos, "chair.dae");

    for (i = 0; i < geos.size(); ++i) {
        collada::MeshGeometry &g = geos[i];

        printf("Geometry id: %s name: %s\n", g.id.c_str(), g.name.c_str());

        for (j = g.sources.begin(); j != g.sources.end(); ++j) {
            const std::string &key = (*j).first;
            const collada::Source &s = (*j).second;
            printf("  Source: %s\n", key.c_str());
            printf("    id: %s\n", s.id.c_str());
            printf("    float_array: %lu elements.\n", s.float_array.size());
            printf("    Accessor:\n");
            printf("      count: %u offset: %u stride: %u\n", s.accessor.count, s.accessor.offset, s.accessor.stride);
            printf("      type: %d\n", s.accessor.type);
            switch (s.accessor.type) {
            case collada::XYZ: 
                printf("      offsets: x: %u y: %u z: %u\n", s.accessor.xyz.x_offset, s.accessor.xyz.y_offset, s.accessor.xyz.z_offset);
                break;
            case collada::ST:
                printf("      offsets: s: %u t: %u\n", s.accessor.st.s_offset, s.accessor.st.t_offset);
                break;
            case collada::RGB:
                printf("      offsets: r: %u g: %u b: %u\n", s.accessor.rgb.r_offset, s.accessor.rgb.g_offset, s.accessor.rgb.b_offset);
                break;
            }
        }

        printf("  Vertices id: %s\n", g.vertices.id.c_str());
        for (k = g.vertices.inputs.begin(); k != g.vertices.inputs.end(); ++k) {
            const std::string &key = (*k).first;
            const collada::SharedInput &value = (*k).second;
            printf("    semantic: %s\n", key.c_str());
            printf("      Input semantic: %s source_id: %s offset: %i set: %i\n",
                   value.semantic.c_str(), value.source_id.c_str(), value.offset, value.set);
        }

        printf("  Polylist count: %u\n", g.polys.count);
        for (k = g.polys.inputs.begin(); k != g.polys.inputs.end(); ++k) {
            const std::string &key = (*k).first;
            const collada::SharedInput &value = (*k).second;
            printf("    semantic: %s\n", key.c_str());
            printf("      Input semantic: %s source_id: %s offset: %i set: %i\n",
                   value.semantic.c_str(), value.source_id.c_str(), value.offset, value.set);
        }
        printf("    vcounts: %lu elements.\n", g.polys.vcounts.size());
        printf("    indices: %lu elements.\n", g.polys.indices.size());

        printf("\n");
    }

    return 0;
}

/*void update(int dt)
{
    g_world->update((float)dt / 1000.0f);
    glutPostRedisplay();
    glutTimerFunc(20, &update, 20);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    g_world->render();
    glutSwapBuffers();
}

void reshape(int new_width, int new_height)
{
    glViewport(0, 0, new_width, new_height);
    g_world->setViewport(new_width, new_height);
}

void keypress(unsigned char key, int x, int y)
{
    switch (key) {
    case 27: // escape key.
        glutLeaveMainLoop();
        break;
    // default:
    //     printf("Key %u was pressed at (%d, %d)\n", key, x, y);
    }
}*/
