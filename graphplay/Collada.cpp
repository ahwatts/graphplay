// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <cstdlib>
#include <sstream>

#include "Collada.h"
#include "tinyxml2.h"

namespace collada {
    using namespace tinyxml2;

    // Loader methods.
    void loadMeshGeometry(MeshGeometry &mgeo, XMLConstHandle mesh_elem);
    void loadSource(Source &source, XMLConstHandle source_elem);
    void loadAccessor(Accessor &acc, XMLConstHandle acc_elem);

    void loadFloatArray(std::vector<float> &array, XMLConstHandle farray_elem);

    // Utility methods.
    void handleError(const char *message);
    void tokenizeStringToFloatArray(std::vector<float> &array, const char *float_string);
    unsigned int getUintAttribute(const XMLElement *node, const char *attr, int default_value);

    // class MeshGeometry.
    MeshGeometry::MeshGeometry() { }

    // TODO: Implement this correctly.
    MeshGeometry::MeshGeometry(const MeshGeometry &other) 
        : sources(other.sources),
          vertices(other.vertices),
          inputs(other.inputs) { }

    MeshGeometry::~MeshGeometry() { }

    // class Source.
    Source::Source() : accessor(*this), float_array() { }

    // TODO: Implement this correctly.
    Source::Source(const Source &other)
        : id(other.id),
          accessor(other.accessor),
          float_array(other.float_array) { }

    Source::~Source() { }

    // class Accessor.
    Accessor::Accessor(const Source &src)
        : source(src),
          count(0),
          offset(0),
          stride(1),
          type(XYZ) {
        xyz.x_offset = 0;
        xyz.y_offset = 1;
        xyz.z_offset = 2;
    }

    float Accessor::getValue(accessor_type_t type, unsigned int index, unsigned int pass) const {
        switch (type) {
        case XYZ:
            if (index < 0 || index > 2) {
                return 0;
            } else {
                return source.float_array[offset + pass*stride + xyz.offsets[index]];
            }
        case ST:
            if (index < 0 || index > 1) {
                return 0;
            } else {
                return source.float_array[offset + pass*stride + st.offsets[index]];
            }
        case RGB:
            if (index < 0 || index > 2) {
                return 0;
            } else {
                return source.float_array[offset + pass*stride + rgb.offsets[index]];
            }
        default:
            return 0;
        }
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
            loadMeshGeometry(mg, node.FirstChildElement("mesh"));
            geos.push_back(mg);
            node = node.NextSiblingElement("geometry");
        }
    }

    void loadMeshGeometry(MeshGeometry &mgeo, XMLConstHandle mesh_elem) {
        XMLConstHandle node(NULL);

        // Load up the sources.
        node = mesh_elem.FirstChildElement("source");
        while (node.ToElement() != NULL) {
            Source src = Source();
            loadSource(src, node);
            mgeo.sources.push_back(src);
            node = node.NextSiblingElement("source");
        }
    }

    void loadSource(Source &source, XMLConstHandle source_elem) {
        if (source_elem.ToElement() != NULL) {
            source.id = source_elem.ToElement()->Attribute("id");
            loadFloatArray(source.float_array, source_elem.FirstChildElement("float_array"));
            loadAccessor(source.accessor, source_elem.FirstChildElement("technique_common").FirstChildElement("accessor"));

            printf("Loaded Source: %s\n", source.id.c_str());
            printf("float_array: %lu elements.\n", source.float_array.size());
            printf("accessor:\n");
            printf("  count: %u offset: %u stride: %u\n", source.accessor.count, source.accessor.offset, source.accessor.stride);
            printf("  type: %d\n", source.accessor.type);
            switch (source.accessor.type) {
            case XYZ: 
                printf("  offsets: x: %u y: %u z: %u\n", source.accessor.xyz.x_offset, source.accessor.xyz.y_offset, source.accessor.xyz.z_offset);
                break;
            case ST:
                printf("  offsets: s: %u t: %u\n", source.accessor.st.s_offset, source.accessor.st.t_offset);
                break;
            case RGB:
                printf("  offsets: r: %u g: %u b: %u\n", source.accessor.rgb.r_offset, source.accessor.rgb.g_offset, source.accessor.rgb.b_offset);
                break;
            }
            printf("\n");
        }
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

    // Utility functions.
    void handleError(const char *message) {
        fprintf(stderr, message);
        exit(1);
    }

    void tokenizeStringToFloatArray(std::vector<float> &array, const char *floats_string) {
        char *curr = (char *)floats_string, *next = (char *)floats_string;
        float this_float = 0;

        this_float = (float)strtod(curr, &next);
        while (curr != next) {
            array.push_back(this_float);
            curr = next;
            this_float = (float)strtod(curr, &next);
        }
    }

    unsigned int getUintAttribute(const XMLElement *node, const char *attr, int default_value) {
        const char *string_value;
        char *next = NULL;
        unsigned int rv;

        if (node == NULL) {
            return default_value;
        }

        string_value = node->Attribute(attr);

        if (string_value == NULL) {
            return default_value;
        }

        rv = strtoul(string_value, &next, 10);

        if (next == string_value) {
            return default_value;
        } else {
            return rv;
        }
    }
};
