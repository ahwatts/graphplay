// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <libxml/xmlversion.h>
#include <libxml/parser.h>

#include "BasicShader.h"
#include "DaeFile.h"
#include "Mesh.h"
#include "World.h"
#include "graphplay.h"

BasicShader *g_shader = NULL;
World *g_world = NULL;

void display();
void keypress(unsigned char key, int x, int y);
void reshape(int new_width, int new_height);
void update(int dt);

int main(int argc, char **argv)
{
    GLenum glew_err;
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
    xmlMemoryDump();

    return 0;
}

void update(int dt)
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
}
