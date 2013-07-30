// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_COLLADA_H_
#define _GRAPHPLAY_GRAPHPLAY_COLLADA_H_

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
        SharedInput();

        std::string semantic, source_id;
        int offset, set;
    };

    class Vertices {
    public:
        typedef std::map<std::string, SharedInput> inputs_t;

        std::string id;
        inputs_t inputs;
    };

    class Polylist {
    public:
        unsigned int vertexCount() const;

        typedef std::map<std::string, SharedInput> inputs_t;
        typedef std::vector<unsigned int> uint_vec_t;

        unsigned int count;
        inputs_t inputs;
        uint_vec_t vcounts, indices;
    };

    class VertexIterator;

    class MeshGeometry {
    public:
        Source *getSource(const std::string &id) const;

        typedef std::map<std::string, Source> sources_t;

        std::string id, name;

        Vertices vertices;
        Polylist polys;

        sources_t sources;

        // iterator BS.
        friend class VertexIterator;
        typedef VertexIterator iterator;
        typedef std::ptrdiff_t difference_type;
        typedef std::size_t size_type;
        typedef std::map<std::string, std::vector<float> > value_type;
        typedef value_type* pointer;
        typedef value_type& reference;

        iterator begin() const;
        iterator end() const;
    };

    class VertexIterator {
    public:
        VertexIterator(const VertexIterator &other);
        VertexIterator(const MeshGeometry &geo, unsigned int init_loc);
        ~VertexIterator(); 

        bool operator==(const VertexIterator &other) const;
        bool operator!=(const VertexIterator &other) const;
        MeshGeometry::value_type operator*();
        VertexIterator &operator++();   // prefix
        VertexIterator operator++(int); // postfix

    private:
        typedef std::map<std::string, const Source *> source_cache_t;

        // You can't create a VertexIterator without a MeshGeometry,
        // nor can you assign one, since geo is a reference and can't
        // be re-assigned.
        VertexIterator();
        VertexIterator &operator=(const VertexIterator &other);

        const Source *getSource(const std::string &id);

        const MeshGeometry &geo;
        unsigned int location;
        source_cache_t source_cache;
    };
};

#endif
