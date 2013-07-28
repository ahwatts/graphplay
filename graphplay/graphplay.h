// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_H_
#define _GRAPHPLAY_H_

#define BUFFER_OFFSET_BYTES(n) ((GLvoid *)n)

#include "BasicShader.h"
#include "Scene.h"

namespace graphplay {
    extern BasicShader *g_shader;
    extern Scene *g_world;
};

#endif
