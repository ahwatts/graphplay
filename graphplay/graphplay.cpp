// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

#include "Collada.h"
#include "Geometry.h"
#include "Material.h"

void display();
void keypress(unsigned char key, int x, int y);
void reshape(int new_width, int new_height);
void update(int dt);

int main(int argc, char **argv)
{
    int width = 800, height = 600, dt_msec = 20;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    glutCreateWindow("graphplay");

    GLenum glew_err = glewInit();
    if (glew_err != GLEW_OK) {
        std::cerr << "Could not initialize GLEW: " << glewGetErrorString(glew_err) << std::endl;
        exit(1);
    }

    std::vector<collada::MeshGeometry> geos;
    collada::loadGeometriesFromFile(geos, "chair.dae");
    graphplay::Geometry geo(geos[0]);
    graphplay::GouraudMaterial mat;

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);

    glutDisplayFunc(display);
    glutKeyboardFunc(keypress);
    glutReshapeFunc(reshape);
    glutTimerFunc(dt_msec, update, dt_msec);
    glutMainLoop();

    return 0;
}

void update(int dt)
{
    // g_world->update((float)dt / 1000.0f);
    glutPostRedisplay();
    glutTimerFunc(20, &update, 20);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // g_world->render();
    glutSwapBuffers();
}

void reshape(int new_width, int new_height)
{
    glViewport(0, 0, new_width, new_height);
    // g_world->setViewport(new_width, new_height);
}

void keypress(unsigned char key, int x, int y)
{
    switch (key) {
    case 27: // escape key.
        glutLeaveMainLoop();
        break;
    }
}
