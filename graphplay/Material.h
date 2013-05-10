#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <GL/glew.h>

class Material
{
public:
    Material();
    virtual ~Material();

    GLuint getPositionLocation();
    GLuint getColorLocation();
};

#endif
