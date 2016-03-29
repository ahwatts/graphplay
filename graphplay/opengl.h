#ifndef _GRAPHPLAY_GRAPHPLAY_OPENGL_H_
#define _GRAPHPLAY_GRAPHPLAY_OPENGL_H_

#include "config.h"

#ifdef __APPLE_CC__
#include <glad/glad.h>
#define GLFW_INCLUDE_GLCOREARB
#include GLFW_HEADER
#else
#include <glad/glad.h>
#include GLFW_HEADER
#endif

#endif
