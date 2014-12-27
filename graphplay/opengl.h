#ifndef _GRAPHPLAY_GRAPHPLAY_OPENGL_H_
#define _GRAPHPLAY_GRAPHPLAY_OPENGL_H_

#include "config.h"

#ifdef __APPLE_CC__
#define GLFW_INCLUDE_GLCOREARB
#include GLFW_HEADER
#else
#include <GL/glew.h>
#include GLFW_HEADER
#endif

#endif
