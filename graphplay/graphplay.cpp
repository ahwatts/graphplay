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

#include <iostream>

#include "Collada.h"
#include "Scene.h"

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

    std::vector<collada::MeshGeometry> geos;
    collada::loadGeometriesFromFile(geos, "chair.dae");

    for (unsigned int i = 0; i < geos.size(); ++i) {
        collada::MeshGeometry &g = geos[i];

        std::cout << "Geometry id: " << g.id << " name: " << g.name << std::endl;

        for (collada::MeshGeometry::sources_t::const_iterator j = g.sources.begin(); j != g.sources.end(); ++j) {
            const collada::Source &s = j->second;
            std::cout << "  Source address: " << &s << std::endl;
            std::cout << "    id: " << s.id << std::endl;
            std::cout << "    float_array: " << s.float_array.size() << " elements." << std::endl;
            std::cout << "    Accessor:" << std::endl;
            std::cout << "     "
                      << " count: " << s.accessor.count
                      << " offset: " << s.accessor.offset
                      << " stride: " << s.accessor.stride
                      << std::endl;
            std::cout << "      type: " << s.accessor.type << std::endl;

            switch (s.accessor.type) {
            case collada::XYZ: 
                std::cout << "      offsets:"
                          << " x: " << s.accessor.xyz.x_offset
                          << " y: " << s.accessor.xyz.y_offset
                          << " z: " << s.accessor.xyz.z_offset
                          << std::endl;
                break;
            case collada::ST:
                std::cout << "      offsets:"
                          << " s: " << s.accessor.st.s_offset
                          << " t: " << s.accessor.st.t_offset
                          << std::endl;
                break;
            case collada::RGB:
                std::cout << "      offsets:"
                          << " r: " << s.accessor.rgb.r_offset
                          << " g: " << s.accessor.rgb.g_offset
                          << " b: " << s.accessor.rgb.b_offset
                          << std::endl;
                break;
            }
        }

        std::cout << "  Vertices id: " << g.vertices.id << std::endl;
        for (collada::Vertices::inputs_t::const_iterator k = g.vertices.inputs.begin(); k != g.vertices.inputs.end(); ++k) {
            const std::string &key = (*k).first;
            const collada::SharedInput &value = (*k).second;
            std::cout << "    semantic: " << key << std::endl;
            std::cout << "      Input semantic: " << value.semantic
                      << " source_id: " << value.source_id
                      << " offset: " << value.offset
                      << " set: " << value.set
                      << std::endl;
        }

        std::cout << "  Polylist count: " << g.polys.count << std::endl;
        for (collada::Polylist::inputs_t::const_iterator k = g.polys.inputs.begin(); k != g.polys.inputs.end(); ++k) {
            const std::string &key = (*k).first;
            const collada::SharedInput &value = (*k).second;
            std::cout << "    semantic: " << key << std::endl;
            std::cout << "      Input semantic: " << value.semantic
                      << " source_id: " << value.source_id
                      << " offset: " << value.offset
                      << " set: " << value.set
                      << std::endl;
        }
        std::cout << "    vcounts: " << g.polys.vcounts.size() << " elements." << std::endl;
        std::cout << "    indices: " << g.polys.indices.size() << " elements." << std::endl;

        collada::MeshGeometry::iterator m = g.begin();
        for (int n = 0; n < 10; ++n) {
            *m;
            ++m;
        }

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
