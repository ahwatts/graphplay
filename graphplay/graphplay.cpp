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

void update(int dt) {
    g_world->update((float)dt / 1000.0f);
    glutPostRedisplay();
    glutTimerFunc(20, &update, 20);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    g_world->render();
    glutSwapBuffers();
}

int main(int argc, char **argv) {
    GLenum glew_err;

    LIBXML_TEST_VERSION;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    glutCreateWindow("graphplay");

    glew_err = glewInit();
    if (glew_err != GLEW_OK) {
        fprintf(stderr, "Could not initialize GLEW: %s\n", glewGetErrorString(glew_err));
        exit(1);
    }

    glEnable(GL_DEPTH_TEST);

    g_shader = new BasicShader();
    g_world = new World(800, 600);

    glutDisplayFunc(&display);
    glutTimerFunc(20, &update, 20);
    glutMainLoop();

    delete g_shader;
    delete g_world;

    xmlCleanupParser();
    xmlMemoryDump();

    return 0;
}
