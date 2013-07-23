// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "Collada.h"
#include "tinyxml2.h"

#ifdef _WIN32
#    define snprintf _snprintf_s
#    define strdup _strdup
#    define strtok_r strtok_s
#endif

namespace collada {
    using namespace tinyxml2;

    // Loader methods.
    Geometry *loadGeometry(XMLConstHandle geo_elem);
    MeshGeometry *loadMeshGeometry(XMLConstHandle mesh_elem);
    Source *loadSource(XMLConstHandle source_elem);
    void loadFloatArray(std::vector<float> &array, XMLConstHandle float_array_elem);

    // Utility methods.
    void handleError(const char *message);
    void tokenizeStringToFloatArray(std::vector<float> &array, char *all_floats);

    // Class Geometry
    Geometry::Geometry() { }
    Geometry::~Geometry() { }

    // Class MeshGeometry
    MeshGeometry::MeshGeometry() : Geometry(), sources() { }
    MeshGeometry::~MeshGeometry() { }

    // Class Accessor
    Accessor::Accessor() : count(0), offset(0), stride(0) { }
    Accessor::~Accessor() { }

    // Class XYZAccessor
    XYZAccessor::XYZAccessor(const Source &s)
        : Accessor(),
          src(s),
          x_offset(0),
          y_offset(1),
          z_offset(2) { }

    XYZAccessor::~XYZAccessor() { }

    float XYZAccessor::getX(unsigned int pass) const {
        return src.float_array[offset + pass*stride + x_offset];
    }

    float XYZAccessor::getY(unsigned int pass) const {
        return src.float_array[offset + pass*stride + y_offset];
    }

    float XYZAccessor::getZ(unsigned int pass) const {
        return src.float_array[offset + pass*stride + z_offset];
    }

    // Class Source
    Source::Source() : accessor(XYZAccessor(*this)), float_array() { }
    Source::~Source() { }

    // Static functions.
    void loadGeometriesFromFile(std::vector<Geometry> &geos, const char* filename) {
        XMLDocument doc;
        XMLConstHandle node(NULL);
        Geometry *geo;

        doc.LoadFile(filename);

        node = XMLConstHandle(&doc)
            .FirstChildElement("COLLADA")
            .FirstChildElement("library_geometries")
            .FirstChildElement("geometry");             

        while (node.ToElement() != NULL) {
            geo = loadGeometry(node);
            if (geo != NULL) { geos.push_back(*geo); }
            node = node.NextSiblingElement("geometry");
        }
    }

    Geometry *loadGeometry(XMLConstHandle geo_elem) {
        Geometry *rv = NULL;
        rv = loadMeshGeometry(geo_elem.FirstChildElement("mesh"));
        // If there is no mesh element, rv will be null. We could use
        // that to try to find other geometry types here...
        return rv;
    }

    MeshGeometry *loadMeshGeometry(XMLConstHandle mesh_elem) {
        XMLConstHandle node(NULL);
        MeshGeometry *rv = new MeshGeometry();
        Source *src;

        // Load up the sources.
        node = mesh_elem.FirstChildElement("source");
        while (node.ToElement() != NULL) {
            src = loadSource(node);
            if (src != NULL) { rv->sources.push_back(*src); }
            node = node.NextSiblingElement("source");
        }

        return rv;
    }

    Source *loadSource(XMLConstHandle source_elem) {
        Source *rv = new Source();
        loadFloatArray(rv->float_array, source_elem.FirstChildElement("float_array"));

        for (unsigned int i = 0; i < rv->float_array.size(); ++i) {
            printf("%g ", rv->float_array[i]);
        }
        printf("\n");

        return rv;
    }

    void loadFloatArray(std::vector<float> &array, XMLConstHandle float_array_elem) {
        int count = -1;
        char *text = NULL;
        const XMLElement *fa_node;
        const XMLText *text_node;

        fa_node = float_array_elem.ToElement();
        if (fa_node != NULL) {
            count = atoi(fa_node->Attribute("count"));
        }

        text_node = float_array_elem.FirstChild().ToText();
        if (text_node != NULL) {
            text = strdup(text_node->Value());
            tokenizeStringToFloatArray(array, text);
            free(text);
        }

        if (count != array.size()) {
            char msg[256];
            snprintf(msg, 256, "Expected %d floats, got %d floats!\n", count, array.size());
            handleError((const char *)msg);
        }
    }

    void handleError(const char *message) {
        fprintf(stderr, message);
        exit(1);
    }

    void tokenizeStringToFloatArray(std::vector<float> &array, char *all_floats) {
        char *this_float = NULL, *next_float = NULL;
        int i = 0;

        this_float = strtok_r(all_floats, " ", &next_float);
        do {
            array.push_back((float)atof(this_float));
            this_float = strtok_r(NULL, " ", &next_float);
        } while (this_float != NULL);
    }
};
