// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _DAE_FILE_H_
#define _DAE_FILE_H_

#include "Mesh.h"

class FloatSource {
public:
    FloatSource();
    FloatSource(int length);
    FloatSource(const FloatSource &other);
    ~FloatSource();

    int length;
    float *data;
};

Mesh* loadDaeFile(const char* filename);

#endif
