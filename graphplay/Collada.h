#ifndef _COLLADA_H_
#define _COLLADA_H_

#include <vector>

namespace collada {
    class Geometry {
    public:
        Geometry();
        virtual ~Geometry();
    };

    class MeshGeometry : public Geometry {
    public:
        MeshGeometry();
        virtual ~MeshGeometry();
    };
    
    void loadGeometriesFromFile(std::vector<Geometry> &geos, const char* filename);
};

#endif
