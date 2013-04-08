#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "BasicShader.h"
#include "World.h"

static BasicShader *g_shader = NULL;
static World *g_world = NULL;

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(g_shader->getProgram());
    glutSwapBuffers();
}

int main(int argc, char **argv) {
    GLenum glew_err;

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

    g_shader = new BasicShader();
    g_world = new World(800, 600);

    glutDisplayFunc(&display);
    glutMainLoop();

    delete g_shader;
    delete g_world;

    return 0;
}
