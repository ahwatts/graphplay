// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _COLLADA_H_
#define _COLLADA_H_

#include <vector>

namespace collada {
    class Geometry;
    class MeshGeometry;
    class Source;
    class Accessor;
    class XYZAccessor;

    // The main factory method to create these objects.
    void loadGeometriesFromFile(std::vector<Geometry> &geos, const char* filename);

    // Accessors for accessing Sources. Only XYZ in Floats is supported.
    class Accessor {
    public:
        unsigned int count, offset, stride;
    };

    class XYZAccessor : public Accessor {
    public:
        XYZAccessor(const Source &s);

        float getX(unsigned int pass) const;
        float getY(unsigned int pass) const;
        float getZ(unsigned int pass) const;

        const Source &src;
        unsigned int x_offset, y_offset, z_offset;
    };

    // Sources. Only sources with float arrays are supported.
    class Source {
    public:
        Accessor accessor;
        std::vector<float> float_array;
    };

    // Geometries and geometry types. Only Mesh geometries are supported.
    class Geometry { };

    class MeshGeometry : public Geometry {
    public:
        std::vector<Source> sources;
    };
};

#endif
