#include "Material.h"

namespace graphplay {
    Material::Material(void)
    { }

    Material::~Material(void)
    { }

    GLuint Material::getPositionLocation() {
        return 0;
    }

    GLuint Material::getColorLocation() {
        return 0;
    }
};
