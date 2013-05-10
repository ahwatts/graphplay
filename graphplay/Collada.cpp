#include "Collada.h"
#include "tinyxml2.h"

namespace collada {
    using namespace tinyxml2;

    Geometry* loadGeometry(const XMLElement &elem);

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

Geometry* loadGeometry(const XMLElement &elem) {
    printf("Parsing geometry name = %s id = %s\n", elem.Attribute("name"), elem.Attribute("id"));
    return NULL;
}

};
