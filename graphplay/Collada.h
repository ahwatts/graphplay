#ifndef _COLLADA_H_
#define _COLLADA_H_

#include <vector>

namespace collada {

    // Geometries and geometry types. Only Mesh geometries are supported.
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

    // Accessors for accessing Sources. Only XYZ in Floats is supported.
    class Source;

    class Accessor {
    public:
        Accessor();
        virtual ~Accessor();

        unsigned int count, offset, stride;
    };

    class XYZAccessor : public Accessor {
    public:
        XYZAccessor();
        virtual ~XYZAccessor();

        float getX(unsigned int pass);
        float getY(unsigned int pass);
        float getZ(unsigned int pass);

        Source *src;
        unsigned int x_offset, y_offset, z_offset;
    };

    class Source {
    public:
        Source();
        virtual ~Source();

        Accessor accessor;
        std::vector<float> float_array;
    };
    
    void loadGeometriesFromFile(std::vector<Geometry> &geos, const char* filename);
};

#endif
