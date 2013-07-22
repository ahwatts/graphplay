// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "Collada.h"
#include "tinyxml2.h"

namespace collada {
    using namespace tinyxml2;

    Geometry *loadGeometry(XMLConstHandle geo_handle);
    MeshGeometry *loadMeshGeometry(XMLConstHandle mesh_handle);
    Source *loadSource(XMLConstHandle source_handle);
    void loadFloatArray(std::vector<float> &array, XMLConstHandle float_array_handle);
    void handleError(const char *message);

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

    void loadGeometriesFromFile(std::vector<Geometry> &geos, const char* filename) {
        XMLDocument doc;
        XMLConstHandle handle(NULL);
        Geometry *geo;

        doc.LoadFile(filename);

        handle = XMLConstHandle(&doc)
            .FirstChildElement("COLLADA")
            .FirstChildElement("library_geometries")
            .FirstChildElement("geometry");

        if (handle.ToElement() == NULL) {
            handleError("Could not find any geometry elements.\n");
        }

        while (handle.ToElement() != NULL) {
            geo = loadGeometry(handle);
            if (geo != NULL) {
                geos.push_back(*geo);
            }
            handle = handle.NextSiblingElement("geometry");
        }
    }

    Geometry *loadGeometry(XMLConstHandle geo_handle) {
        XMLConstHandle handle(NULL);
        Geometry *rv;

        handle = geo_handle.FirstChildElement("mesh");

        if (handle.ToElement() != NULL) {
            if (strcmp(handle.ToElement()->Name(), "mesh") == 0) {
                rv = loadMeshGeometry(handle);
            } else {
                handleError("Don't know how to handle a non-mesh geometry child!\n");
            }
        } else {
            handleError("Geometry node has no children!\n");
        }

        return rv;
    }

    MeshGeometry *loadMeshGeometry(XMLConstHandle mesh_handle) {
        XMLConstHandle handle(NULL);
        MeshGeometry *rv = new MeshGeometry();
        Source *src;

        handle = mesh_handle.FirstChildElement("source");
        if (handle.ToElement()) {
            src = loadSource(handle);
            if (src) { rv->sources.push_back(*src); }
        } else {
            handleError("Mesh node has no sources!\n");
        }

        return rv;
    }

    Source *loadSource(XMLConstHandle source_handle) {
        XMLConstHandle handle(NULL);
        Source *rv = new Source();

        handle = source_handle.FirstChildElement("float_array");
        if (handle.ToElement()) {
            loadFloatArray(rv->float_array, handle);
        } else {
            handleError("Source node has no float_array!\n");
        }

        return rv;
    }

    void loadFloatArray(std::vector<float> &array, XMLConstHandle float_array_handle) {
        const char *text;
        const XMLText *text_node;

        text_node = float_array_handle.FirstChild().ToText();
        if (text_node) {
            text = text_node->Value();
            printf("Parsing float array: %s\n", text);
        } else {
            handleError("Float array node has no values?!\n");
        }
    }

    void handleError(const char *message) {
        fprintf(stderr, message);
        exit(1);
    }
};
