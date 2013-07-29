// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <iostream>
#include <numeric>
#include <sstream>

#include "Collada.h"
#include "tinyxml2.h"

namespace collada {
    using namespace tinyxml2;

    typedef std::vector<float> float_vec_t;

    // Loader methods.
    void loadMeshGeometry(MeshGeometry &mgeo, XMLConstHandle mesh_elem);
    void loadSource(Source &source, XMLConstHandle source_elem);
    void loadFloatArray(float_vec_t &array, XMLConstHandle farray_elem);
    void loadAccessor(Accessor &acc, XMLConstHandle acc_elem);
    void loadVertices(Vertices &vers, XMLConstHandle verts_elem);
    void loadSharedInput(SharedInput &input, XMLConstHandle input_elem);
    void loadPolylist(Polylist &polys, XMLConstHandle poly_elem);

    // Utility stuff.

    // Prints message and exits.
    void handleError(const char *message);

    // Wraps a function pointer to strtol, strtod, etc. Handles the
    // difference in arity between the floating-point and integral
    // methods by statically passing a base of 10 to the integral ones.
    template<class T>
    class ConversionFunc {
    public:
        ConversionFunc(T (*strtointegral)(const char *, char **, int)) : integral(strtointegral), floating(NULL) { }
        ConversionFunc(T (*strtofloating)(const char *, char **)) : integral(NULL), floating(strtofloating) { }

        T operator()(const char * str, char **nptr) const {
            if (integral != NULL) {
                return integral(str, nptr, 10);
            } else if (floating != NULL) {
                return floating(str, nptr);
            } else {
                return 0;
            }
        }

    private:
        T (*integral)(const char *, char **, int);
        T (*floating)(const char *, char **);
    };

    static const ConversionFunc<double> cf_strtod(&strtod);
    static const ConversionFunc<unsigned long> cf_strtoul(&strtoul);
    static const ConversionFunc<long> cf_strtol(&strtol);

    // Converts each value in string with convert, storing it to array.
    template<class T, class IT>
    class tokenizeStringToArray {
    public:
        tokenizeStringToArray(const ConversionFunc<IT> &func) : convert(func) { }

        void operator()(std::vector<T> &array, const char *string) const {
            char *curr = (char *)string, *next = (char *)string;
            T this_value = 0;

            this_value = (T)convert(curr, &next);

            while (curr != next) {
                array.push_back(this_value);
                curr = next;
                this_value = (T)convert(curr, &next);
            }
        }

    private:
        const ConversionFunc<IT> &convert;
    };

    static const tokenizeStringToArray<float, double> tokenizeStringToFloatArray(cf_strtod);
    static const tokenizeStringToArray<unsigned int, unsigned long> tokenizeStringToUintArray(cf_strtoul);

    // Retrives attr from node, converts it with convert. If anything
    // goes awry (e.g. node is null, node doesn't have attr, the value
    // isn't numeric), returns default_value.
    template<class T, class IT>
    class getNumericAttribute {
    public:
        getNumericAttribute(const ConversionFunc<IT> &func) : convert(func) { }

        T operator()(const XMLElement *node, const char *attr, T default_value) const {
            const char *string_value;
            char *next = NULL;
            T rv;

            if (node == NULL) {
                return default_value;
            }

            string_value = node->Attribute(attr);

            if (string_value == NULL) {
                return default_value;
            }

            rv = (T)convert(string_value, &next);

            if (next == string_value) {
                return default_value;
            } else {
                return rv;
            }
        }

    private:
        const ConversionFunc<IT> &convert;
    };

    static const getNumericAttribute<unsigned int, unsigned long> getUintAttribute(cf_strtoul);
    static const getNumericAttribute<int, long> getIntAttribute(cf_strtol);
    static const getNumericAttribute<float, double> getFloatAttribute(cf_strtod);

    // class MeshGeometry.
    void MeshGeometry::resolveSources() {
        for (Polylist::inputs_t::iterator i = polys.inputs.begin(); i != polys.inputs.end(); ++i) {
            SharedInput &in = i->second;

            // Strip off the leading #.
            sources_t::iterator s = sources.find(in.source_id.substr(1));
            if (s != sources.end()) {
                in.source = &s->second;
            }
        }
    }

    MeshGeometry::iterator MeshGeometry::begin() const {
        return MeshGeometry::iterator(*this, 0);
    }

    MeshGeometry::iterator MeshGeometry::end() const {
        return MeshGeometry::iterator(*this, polys.vertexCount());
    }

    // class VertexIterator, which iterates over the vertices in a
    // MeshGeometry.
    VertexIterator::VertexIterator(const MeshGeometry &container, unsigned int init_loc)
        : geo(container), location(init_loc) { }

    bool VertexIterator::operator==(const VertexIterator &other) const {
        return !(*this != other);
    }

    bool VertexIterator::operator!=(const VertexIterator &other) const {
        return (this != &other) || (location != other.location);
    }

    MeshGeometry::value_type VertexIterator::operator*() const {
        MeshGeometry::value_type rv;

        // unsigned int nattrs = geo.polys.indices.size() / geo.polys.vertexCount();
        // unsigned int offset = location * nattrs;

        // std::cout << "location = " << location << " offset = " << offset
        //           << " nattrs = " << nattrs << " #verts = " << geo.polys.vertexCount()
        //           << " #indices = " << geo.polys.indices.size()
        //           << std::endl;

        // MeshGeometry::inputs_t::const_iterator i;
        // for (i = geo.polys.inputs.begin(); i != geo.polys.inputs.end(); ++i) {
        //     const std::string &semantic = i->first;
        //     std::cout << "location = " << location << " semantic = " << semantic << std::endl;
        // }

        return rv;
    }

    VertexIterator &VertexIterator::operator++() {
        // pre-increment.
        ++location;
        return *this;
    }

    VertexIterator VertexIterator::operator++(int) {
        // post-increment.
        VertexIterator clone(*this);
        ++location;
        return clone;
    }

    // class Source.
    Source::Source() : accessor(*this), float_array() { }

    Source::Source(const Source &other)
        : id(other.id),
          accessor(other.accessor, *this),
          float_array(other.float_array) { }

    Source::~Source() { }

    Source &Source::operator=(const Source &other) {
        id = other.id;
        accessor.set(other.accessor, *this);
        float_array = other.float_array;
        return *this;
    }

    // class Accessor.
    Accessor::Accessor(const Source &src)
        : source(&src),
          count(0),
          offset(0),
          stride(1),
          type(XYZ) {
        xyz.x_offset = 0;
        xyz.y_offset = 1;
        xyz.z_offset = 2;
    }

    Accessor::Accessor(const Accessor &other, const Source &src) {
        set(other, src);
    }

    Accessor &Accessor::set(const Accessor &other, const Source &src) {
        source = &src;
        count = other.count;
        offset = other.offset;
        stride = other.stride;
        type = other.type;
        switch (type) {
        case XYZ: std::copy(other.xyz.offsets, other.xyz.offsets + 3, xyz.offsets); break;
        case ST:  std::copy(other.st.offsets,  other.st.offsets  + 2, st.offsets);  break;
        case RGB: std::copy(other.rgb.offsets, other.rgb.offsets + 3, rgb.offsets); break;
        }

        return *this;
    }

    float Accessor::getValue(accessor_type_t type, unsigned int index, unsigned int pass) const {
        switch (type) {
        case XYZ:
            if (index < 0 || index > 2) {
                return 0;
            } else {
                return source->float_array[offset + pass*stride + xyz.offsets[index]];
            }
        case ST:
            if (index < 0 || index > 1) {
                return 0;
            } else {
                return source->float_array[offset + pass*stride + st.offsets[index]];
            }
        case RGB:
            if (index < 0 || index > 2) {
                return 0;
            } else {
                return source->float_array[offset + pass*stride + rgb.offsets[index]];
            }
        default:
            return 0;
        }
    }

    // class SharedInput.
    SharedInput::SharedInput() : semantic(), source_id(), offset(0), set(-1), source(NULL) { }

    // class PolyList.
    unsigned int Polylist::vertexCount() const {
        return std::accumulate<uint_vec_t::const_iterator, unsigned int>
            (vcounts.begin(), vcounts.end(), 0);
    }

    // Static functions.
    void loadGeometriesFromFile(std::vector<MeshGeometry> &geos, const char* filename) {
        XMLDocument doc;
        XMLConstHandle node(NULL);

        doc.LoadFile(filename);

        node = XMLConstHandle(&doc)
            .FirstChildElement("COLLADA")
            .FirstChildElement("library_geometries")
            .FirstChildElement("geometry");

        while (node.ToElement() != NULL && node.FirstChildElement("mesh").ToElement() != NULL) {
            MeshGeometry mg = MeshGeometry();
            mg.id = node.ToElement()->Attribute("id");
            mg.name = node.ToElement()->Attribute("name");
            loadMeshGeometry(mg, node.FirstChildElement("mesh"));
            geos.push_back(mg);
            node = node.NextSiblingElement("geometry");
        }
    }

    void loadMeshGeometry(MeshGeometry &mgeo, XMLConstHandle mesh_elem) {
        const XMLElement *mesh_node = mesh_elem.ToElement();
        XMLConstHandle node(NULL);

        if (mesh_node != NULL) {
            // Load up the sources.
            node = mesh_elem.FirstChildElement("source");
            while (node.ToElement() != NULL) {
                Source src = Source();
                loadSource(src, node);
                mgeo.sources[src.id] = src;
                node = node.NextSiblingElement("source");
            }

            // Load up the vertices element.
            loadVertices(mgeo.vertices, mesh_elem.FirstChildElement("vertices"));

            // Load up the polylist element.
            loadPolylist(mgeo.polys, mesh_elem.FirstChildElement("polylist"));
        }

        mgeo.resolveSources();
    }

    void loadSource(Source &source, XMLConstHandle source_elem) {
        if (source_elem.ToElement() != NULL) {
            source.id = source_elem.ToElement()->Attribute("id");
            loadFloatArray(source.float_array, source_elem.FirstChildElement("float_array"));
            loadAccessor(source.accessor, source_elem.FirstChildElement("technique_common").FirstChildElement("accessor"));
        }
    }

    void loadFloatArray(float_vec_t &array, XMLConstHandle float_array_elem) {
        int count = -1;
        const XMLElement *fa_node;
        const XMLText *text_node;

        fa_node = float_array_elem.ToElement();
        if (fa_node != NULL) {
            count = getIntAttribute(fa_node, "count", 0);
        }

        text_node = float_array_elem.FirstChild().ToText();
        if (text_node != NULL) {
            tokenizeStringToFloatArray(array, text_node->Value());
        }

        if (count != (int)array.size()) {
            std::ostringstream msg;
            msg << "Expected " << count << " floats, got " << array.size() << " floats!\n";
            handleError(msg.str().c_str());
        }
    }

    void loadAccessor(Accessor &accessor, XMLConstHandle accessor_elem) {
        std::string param_names, param_name;
        XMLConstHandle node(NULL);
        const XMLElement *acc_node = accessor_elem.ToElement();

        if (acc_node != NULL) {
            accessor.count = getUintAttribute(acc_node, "count", 0);
            accessor.offset = getUintAttribute(acc_node, "offset", 0);
            accessor.stride = getUintAttribute(acc_node, "stride", 1);

            node = accessor_elem.FirstChildElement("param");
            while (node.ToElement() != NULL) {
                param_names += node.ToElement()->Attribute("name");
                node = node.NextSiblingElement("param");
            }

            if (param_names.compare("XYZ") == 0) {
                accessor.type = XYZ;

                node = accessor_elem.FirstChildElement("param");
                while (node.ToElement() != NULL) {
                    param_name = node.ToElement()->Attribute("name");

                    if (param_name.compare("X") == 0) {
                        accessor.xyz.x_offset = getUintAttribute(node.ToElement(), "offset", 0);
                    } else if (param_name.compare("Y") == 0) {
                        accessor.xyz.y_offset = getUintAttribute(node.ToElement(), "offset", 1);
                    } else if (param_name.compare("Z") == 0) {
                        accessor.xyz.z_offset = getUintAttribute(node.ToElement(), "offset", 2);
                    }

                    node = node.NextSiblingElement("param");
                }
            } else if (param_names.compare("ST") == 0) {
                accessor.type = ST;

                node = accessor_elem.FirstChildElement("param");
                while (node.ToElement() != NULL) {
                    param_name = node.ToElement()->Attribute("name");

                    if (param_name.compare("S") == 0) {
                        accessor.st.s_offset = getUintAttribute(node.ToElement(), "offset", 0);
                    } else if (param_name.compare("T") == 0) {
                        accessor.st.t_offset = getUintAttribute(node.ToElement(), "offset", 1);
                    }

                    node = node.NextSiblingElement("param");
                }
            } else if (param_names.compare("RGB") == 0) {
                accessor.type = RGB;

                node = accessor_elem.FirstChildElement("param");
                while (node.ToElement() != NULL) {
                    param_name = node.ToElement()->Attribute("name");

                    if (param_name.compare("R") == 0) {
                        accessor.rgb.r_offset = getUintAttribute(node.ToElement(), "offset", 0);
                    } else if (param_name.compare("G") == 0) {
                        accessor.rgb.g_offset = getUintAttribute(node.ToElement(), "offset", 1);
                    } else if (param_name.compare("B") == 0) {
                        accessor.rgb.b_offset = getUintAttribute(node.ToElement(), "offset", 2);
                    }

                    node = node.NextSiblingElement("param");
                }
            }
        }
    }

    void loadVertices(Vertices &verts, XMLConstHandle verts_elem) {
        XMLConstHandle node(NULL);

        if (verts_elem.ToElement() != NULL) {
            verts.id = verts_elem.ToElement()->Attribute("id");

            node = verts_elem.FirstChildElement("input");
            while (node.ToElement() != NULL) {
                SharedInput s;
                loadSharedInput(s, node);
                verts.inputs[s.semantic] = s;
                node = node.NextSiblingElement("input");
            }
        }
    }

    void loadSharedInput(SharedInput &input, XMLConstHandle input_elem) {
        const XMLElement *node = input_elem.ToElement();

        if (node != NULL) {
            input.semantic = node->Attribute("semantic");
            input.source_id = node->Attribute("source");
            input.offset = getIntAttribute(node, "offset", -1);
            input.set = getIntAttribute(node, "set", -1);
        }
    }

    void loadPolylist(Polylist &polys, XMLConstHandle poly_elem) {
        const XMLElement *pnode = poly_elem.ToElement();
        XMLConstHandle node(NULL);

        if (pnode != NULL) {
            polys.count = getUintAttribute(pnode, "count", 0);

            for (node = poly_elem.FirstChildElement("input");
                 node.ToElement() != NULL;
                 node = node.NextSiblingElement("input")) {
                SharedInput s;
                loadSharedInput(s, node);
                polys.inputs[s.semantic] = s;
            }

            const XMLText *vc = poly_elem.FirstChildElement("vcount").FirstChild().ToText();
            if (vc != NULL) {
                tokenizeStringToUintArray(polys.vcounts, vc->Value());
            }

            const XMLText *p = poly_elem.FirstChildElement("p").FirstChild().ToText();
            if (p != NULL) {
                tokenizeStringToUintArray(polys.indices, p->Value());
            }
        }
    }

    // Utility functions.
    void handleError(const char *message) {
        fprintf(stderr, message);
        exit(1);
    }
};
