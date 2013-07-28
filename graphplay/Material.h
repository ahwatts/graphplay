#ifndef _GRAPHPLAY_GRAPHPLAY_MATERIAL_H_
#define _GRAPHPLAY_GRAPHPLAY_MATERIAL_H_

#include <GL/glew.h>

namespace graphplay {
    class Material
    {
    public:
        Material();
        virtual ~Material();

        GLuint getPositionLocation();
        GLuint getColorLocation();
    };
};

#endif
