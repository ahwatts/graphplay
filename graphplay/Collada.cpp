// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <sstream>

#include "Collada.h"
#include "tinyxml2.h"

namespace collada {
    using namespace tinyxml2;

    // Loader methods.
    Geometry *loadGeometry(XMLConstHandle geo_elem);
    MeshGeometry *loadMeshGeometry(XMLConstHandle mesh_elem);
    Source *loadSource(XMLConstHandle source_elem);
    void loadFloatArray(std::vector<float> &array, XMLConstHandle float_array_elem);
    void loadAccessor(Source *source, XMLConstHandle accessor_elem);

    // Utility methods.
    void handleError(const char *message);
    void tokenizeStringToFloatArray(std::vector<float> &array, const char *float_string);
    unsigned int getUintAttribute(const XMLElement *node, const char *attr, int default_value);

    // class Accessor.
    Accessor::Accessor(const Source *src)
        : source(src),
          count(0),
          offset(0),
          stride(1),
          type(XYZ) {
        xyz.x_offset = 0;
        xyz.y_offset = 0;
        xyz.z_offset = 0;
    }

    float Accessor::getValue(accessor_type_t type, unsigned int index, unsigned int pass) const {
        if (source == NULL) { return 0; }

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
        default:
            return 0;
        }
    }

    // class Source.
    Source::Source() : accessor(this), float_array() { }

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
        return rv;
    }

    void loadFloatArray(std::vector<float> &array, XMLConstHandle float_array_elem) {
        int count = -1;
        const XMLElement *fa_node;
        const XMLText *text_node;

        fa_node = float_array_elem.ToElement();
        if (fa_node != NULL) {
            count = atoi(fa_node->Attribute("count"));
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

    void loadAccessor(Source *source, XMLConstHandle accessor_elem) {
        std::string param_names, param_name;
        XMLConstHandle node(NULL);
        const XMLElement *acc_node = accessor_elem.ToElement();

        if (acc_node != NULL) {
            source->accessor.count = getUintAttribute(acc_node, "count", 0);
            source->accessor.offset = getUintAttribute(acc_node, "offset", 0);
            source->accessor.stride = getUintAttribute(acc_node, "stride", 1);

            node = accessor_elem.FirstChildElement("param");
            while (node.ToElement() != NULL) {
                param_names += node.ToElement()->Attribute("name");
                node = node.NextSiblingElement("param");
            }

            if (param_names.compare("XYZ") == 0) {
                source->accessor.type = XYZ;

                node = accessor_elem.FirstChildElement("param");
                while (node.ToElement() != NULL) {
                    param_name = node.ToElement()->Attribute("name");

                    if (param_name.compare("X") == 0) {
                        source->accessor.xyz.x_offset = getUintAttribute(node.ToElement(), "offset", 0);
                    } else if (param_name.compare("Y") == 0) {
                        source->accessor.xyz.y_offset = getUintAttribute(node.ToElement(), "offset", 1);
                    } else if (param_name.compare("Z") == 0) {
                        source->accessor.xyz.z_offset = getUintAttribute(node.ToElement(), "offset", 2);
                    }

                    node = node.NextSiblingElement("param");
                }
            } else if (param_names.compare("ST")) {
                source->accessor.type = ST;

                node = accessor_elem.FirstChildElement("param");
                while (node.ToElement() != NULL) {
                    param_name = node.ToElement()->Attribute("name");

                    if (param_name.compare("S") == 0) {
                        source->accessor.st.s_offset = getUintAttribute(node.ToElement(), "offset", 0);
                    } else if (param_name.compare("T") == 0) {
                        source->accessor.st.t_offset = getUintAttribute(node.ToElement(), "offset", 1);
                    }

                    node = node.NextSiblingElement("param");
                }
            }
        }
    }

    // Utility functions.
    void handleError(const char *message) {
        fprintf(stderr, message);
        exit(1);
    }

    void tokenizeStringToFloatArray(std::vector<float> &array, const char *floats_string) {
        char *curr = (char *)floats_string, *next = (char *)floats_string;
        float this_float = 0;

        this_float = strtof(curr, &next);
        while (curr != next) {
            array.push_back(this_float);
            curr = next;
            this_float = strtof(curr, &next);
        }
    }

    unsigned int getUintAttribute(const XMLElement *node, const char *attr, int default_value) {
        const char *string_value;
        unsigned int rv;

        if (node == NULL) {
            return default_value;
        }

        string_value = node->Attribute(attr);

        if (string_value == NULL) {
            return default_value;
        }

        rv = strtoul(string_value, NULL, 10);

        return rv;
    }
};
