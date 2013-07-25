// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <algorithm>
#include <cstdlib>
#include <sstream>

#include "Collada.h"
#include "tinyxml2.h"

namespace collada {
    using namespace tinyxml2;

    // Loader methods.
    void loadMeshGeometry(MeshGeometry &mgeo, XMLConstHandle mesh_elem);
    void loadSource(Source &source, XMLConstHandle source_elem);
    void loadFloatArray(std::vector<float> &array, XMLConstHandle farray_elem);
    void loadAccessor(Accessor &acc, XMLConstHandle acc_elem);
    void loadVertices(Vertices &vers, XMLConstHandle verts_elem);
    void loadSharedInput(SharedInput &input, XMLConstHandle input_elem);

    // Utility methods.
    void handleError(const char *message);
    void tokenizeStringToFloatArray(std::vector<float> &array, const char *float_string);
    int          getIntAttribute( const XMLElement *node, const char *attr, int default_value);
    unsigned int getUintAttribute(const XMLElement *node, const char *attr, unsigned int default_value);

    // class MeshGeometry.
    MeshGeometry::MeshGeometry() { }

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
                mgeo.sources.push_back(src);
                node = node.NextSiblingElement("source");
            }

            // Load up the vertices element.
            loadVertices(mgeo.vertices, mesh_elem.FirstChildElement("vertices"));
        }
    }

    void loadSource(Source &source, XMLConstHandle source_elem) {
        if (source_elem.ToElement() != NULL) {
            source.id = source_elem.ToElement()->Attribute("id");
            loadFloatArray(source.float_array, source_elem.FirstChildElement("float_array"));
            loadAccessor(source.accessor, source_elem.FirstChildElement("technique_common").FirstChildElement("accessor"));
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

    unsigned int getUintAttribute(const XMLElement *node, const char *attr, unsigned int default_value) {
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

    int getIntAttribute(const XMLElement *node, const char *attr, int default_value) {
        const char *string_value;
        char *next = NULL;
        int rv;

        if (node == NULL) {
            return default_value;
        }

        string_value = node->Attribute(attr);

        if (string_value == NULL) {
            return default_value;
        }

        rv = strtol(string_value, &next, 10);

        if (next == string_value) {
            return default_value;
        } else {
            return rv;
        }
    }
};
