// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _COLLADA_H_
#define _COLLADA_H_

#include <map>
#include <string>
#include <vector>

namespace collada {
    class MeshGeometry;
    class Source;
    class Accessor;
    class XYZAccessor;

    // The main factory method to create these objects.
    void loadGeometriesFromFile(std::vector<MeshGeometry> &geos, const char *filename);

    // Accessors for accessing Sources.
    enum accessor_type_t { XYZ, ST, RGB };
    
    class Accessor {
    public:
        Accessor(const Source &src);

        const Source &source;
        unsigned int count, offset, stride;

        inline float getX(unsigned int pass) const { return getValue(XYZ, 0, pass); };
        inline float getY(unsigned int pass) const { return getValue(XYZ, 1, pass); };
        inline float getZ(unsigned int pass) const { return getValue(XYZ, 2, pass); };

        inline float getS(unsigned int pass) const { return getValue(ST, 0, pass); };
        inline float getT(unsigned int pass) const { return getValue(ST, 1, pass); };

        inline float getR(unsigned int pass) const { return getValue(RGB, 0, pass); };
        inline float getG(unsigned int pass) const { return getValue(RGB, 1, pass); };
        inline float getB(unsigned int pass) const { return getValue(RGB, 2, pass); };

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

            union {
                struct { unsigned int r_offset, g_offset, b_offset; };
                unsigned int offsets[3];
            } rgb;
        };

    private:
        float getValue(accessor_type_t type, unsigned int index, unsigned int pass) const;
    };

    // Sources. Only sources with float arrays are supported.
    class Source {
    public:
        Source();
        Source(const Source &other);
        virtual ~Source();

        Source &operator=(const Source &other);

        std::string id;
        Accessor accessor;
        std::vector<float> float_array;
    };

    class SharedInput {
    public:
        unsigned int offset, set;
        Source *source;
    };

    class Vertices {
    public:
        std::string id;
        std::map<std::string, SharedInput> inputs;
    };

    // Geometries and geometry types. Only Mesh geometries are supported.
    class MeshGeometry {
    public:
        MeshGeometry();
        MeshGeometry(const MeshGeometry &other);
        virtual ~MeshGeometry();

        MeshGeometry &operator=(const MeshGeometry &other);

        std::vector<Source> sources;
        Vertices vertices;
        std::map<std::string, SharedInput> inputs;
    };
};

#endif
