#include "Collada.h"
#include "tinyxml2.h"

namespace collada {
    using namespace tinyxml2;

    Geometry* loadGeometry(const XMLElement &elem);
    MeshGeometry* loadMeshGeometry(const XMLElement &elem);

    Geometry::Geometry()
    { }

    Geometry::~Geometry()
    { }

    MeshGeometry::MeshGeometry()
        : Geometry()
    { }

    MeshGeometry::~MeshGeometry()
    { }

    void loadGeometriesFromFile(std::vector<Geometry> &geos, const char* filename) {
        XMLDocument doc;
        XMLElement *elem;
        Geometry *geo;

        doc.LoadFile(filename);

        elem = doc.FirstChildElement("COLLADA");
        if (elem == NULL) {
            fprintf(stderr, "Could not find a COLLADA element in the document.\n");
            exit(1);
        }

        elem = elem->FirstChildElement("library_geometries");
        if (elem == NULL) {
            fprintf(stderr, "Could not find library_geometries in the COLLADA element.\n");
            exit(1);
        }

        elem = elem->FirstChildElement("geometry");
        while (elem != NULL) {
            geo = loadGeometry(*elem);
            if (geo != NULL) {
                geos.push_back(*geo);
            }
            elem = elem->NextSiblingElement("geometry");
        }
    }

    Geometry* loadGeometry(const XMLElement &geo_elem) {
        const XMLElement *elem;
        Geometry *rv;

        printf("Parsing geometry name = %s id = %s\n", geo_elem.Attribute("name"), geo_elem.Attribute("id"));

        elem = geo_elem.FirstChildElement();

        if (elem) {
            if (strcmp(elem->Name(), "mesh") == 0) {
                rv = loadMeshGeometry(*elem);
            } else {
                fprintf(stderr, "Don't know how to handle a geometry chid of type %s!\n", elem->Name());
                exit(1);
            }
        } else {
            fprintf(stderr, "Geometry node has no children!\n");
            exit(1);
        }

        return rv;
    }

    MeshGeometry* loadMeshGeometry(const XMLElement &mesh_elem) {
        return NULL;
    }
};
