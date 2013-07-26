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

        // If we're going to copy an Accessor, it's usually going to
        // be because we're copying the Source, so we're going to want
        // to copy the Accessor, but attach a different Source to it.
        Accessor(const Accessor &other, const Source &source);

        // Assignment but with a different Source.
        Accessor &set(const Accessor &other, const Source &source);

        inline float getX(unsigned int pass) const { return getValue(XYZ, 0, pass); };
        inline float getY(unsigned int pass) const { return getValue(XYZ, 1, pass); };
        inline float getZ(unsigned int pass) const { return getValue(XYZ, 2, pass); };

        inline float getS(unsigned int pass) const { return getValue(ST, 0, pass); };
        inline float getT(unsigned int pass) const { return getValue(ST, 1, pass); };

        inline float getR(unsigned int pass) const { return getValue(RGB, 0, pass); };
        inline float getG(unsigned int pass) const { return getValue(RGB, 1, pass); };
        inline float getB(unsigned int pass) const { return getValue(RGB, 2, pass); };

        const Source *source;
        unsigned int count, offset, stride;
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

        // These should not be used, since a copied Accessor needs to
        // point to a different Source.
        Accessor(const Accessor &other);
        Accessor &operator=(const Accessor &other);
    };

    // Sources. Only sources with float arrays are supported.
    class Source {
    public:
        Source();
        Source(const Source &other);
        ~Source();

        Source &operator=(const Source &other);

        std::string id;
        Accessor accessor;
        std::vector<float> float_array;
    };

    class SharedInput {
    public:
        std::string semantic, source_id;
        int offset, set;
        const Source *source;
    };

    class Vertices {
    public:
        std::string id;
        std::map<std::string, SharedInput> inputs;
    };

    class Polylist {
    public:
        unsigned int count;
        std::map<std::string, SharedInput> inputs;
        std::vector<unsigned int> vcounts, indices;
    };

    // Geometries and geometry types. Only Mesh geometries are supported.
    class MeshGeometry {
    public:
        MeshGeometry();

        std::string id, name;
        std::vector<Source> sources;
        Vertices vertices;
        std::map<std::string, SharedInput> inputs;
        Polylist polys;
    };
};

#endif
