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

    // Accessors for accessing Sources.
    enum accessor_type_t { XYZ, ST };
    
    class Accessor {
    public:
        Accessor(const Source *src);

        const Source *source;
        unsigned int count, offset, stride;

        inline float getX(unsigned int pass) const { return getValue(XYZ, 0, pass); };
        inline float getY(unsigned int pass) const { return getValue(XYZ, 1, pass); };
        inline float getZ(unsigned int pass) const { return getValue(XYZ, 2, pass); };

        inline float getS(unsigned int pass) const { return getValue(ST, 0, pass); };
        inline float getT(unsigned int pass) const { return getValue(ST, 1, pass); };

        accessor_type_t type;
        union {
            union {
                struct { unsigned int x_offset, y_offset, z_offset; };
                unsigned int offsets[3];
            } xyz;

            union {
                struct { unsigned int s_offset, t_offset; };
                unsigned int offsets[2];
            } st;
        };

    private:
        float getValue(accessor_type_t type, unsigned int index, unsigned int pass) const;
    };

    // Sources. Only sources with float arrays are supported.
    class Source {
    public:
        Source();

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
