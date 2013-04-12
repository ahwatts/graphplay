// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <map>
#include <string>
#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "DaeFile.h"

#ifdef _WIN32
#define snprintf _snprintf_s
#define strdup _strdup
#define strtok_r strtok_s
#endif

xmlXPathContextPtr createDaeFileXPathContext(xmlDocPtr doc);
void loadGeometry(xmlNodePtr node);

DaeFile* DaeFile::loadDaeFile(const char* filename) {
    xmlDocPtr doc = NULL;
    xmlNodePtr node = NULL;
    xmlXPathContextPtr xpath_ctxt = NULL;
    xmlXPathObjectPtr xpath_result = NULL;

    // Load up the XML document.
    doc = xmlParseFile(filename);
    if (doc == NULL) {
        fprintf(stderr, "Failed to read XML file %s\n", filename);
        exit(1);
    }

    // Create a context for querying it with XPath.
    xpath_ctxt = createDaeFileXPathContext(doc);

    // Do the XPath search.
    xpath_result = xmlXPathEvalExpression(BAD_CAST "//dae:geometry", xpath_ctxt);

    if (xpath_result && !xmlXPathNodeSetIsEmpty(xpath_result->nodesetval)) {
        printf("Found %d nodes!\n", xmlXPathNodeSetGetLength(xpath_result->nodesetval));
        for (int i = 0; i < xmlXPathNodeSetGetLength(xpath_result->nodesetval); ++i) {
            node = xpath_result->nodesetval->nodeTab[i];
            printf("found node: name = %s\n", node->name);
            loadGeometry(node);
        }
    }

    xmlXPathFreeObject(xpath_result);
    xmlXPathFreeContext(xpath_ctxt);
    xmlFreeDoc(doc);

    return NULL;
}

DaeFile::DaeFile(void) { }

DaeFile::~DaeFile(void) { }

xmlXPathContextPtr createDaeFileXPathContext(xmlDocPtr doc) {
    xmlXPathContextPtr xpath_ctxt = xmlXPathNewContext(doc);

    // Make sure the context was created.
    if (xpath_ctxt == NULL) {
        fprintf(stderr, "Failed to create XPath context!\n");
        exit(1);
    }

    // Register a prefix for the default namespace (which doesn't have one, at
    // least in the document I have...) so that XPath can actually find the nodes.
    xmlXPathRegisterNs(xpath_ctxt, BAD_CAST "dae", doc->xmlRootNode->ns->href);

    return xpath_ctxt;
}

/*void loadPolyList(xmlNodePtr pl_node, xmlNodePtr mesh_node) {
    num_polys = atoi((char *)xmlGetProp(polylist, BAD_CAST "count"));
    printf("Polylist has %d polys.\n", num_polys);
}*/

class FloatSource {
public:
    FloatSource();
    FloatSource(int length);
    FloatSource(const FloatSource &other);
    ~FloatSource();

    int length;
    float *data;
};

FloatSource::FloatSource()
    : length(0),
      data(0)
{ }

FloatSource::FloatSource(int length)
    : length(length),
      data(new float[length])
{ }

FloatSource::FloatSource(const FloatSource &other)
    : length(other.length),
      data(new float[other.length])
{
    memcpy(data, other.data, length*sizeof(float));
}

FloatSource::~FloatSource() {
    if (data) {
        delete [] data;
    }
}

FloatSource *loadSource(xmlNodePtr source_node) {
    FloatSource *rv = NULL;
    int num_floats, i;
    xmlNodePtr farray;
    char *all_floats, *this_float, *next_float, *prop;

    // We're going to ignore the accessor element and assume that the data
    // is in blocks of 3 going like x, y, z.
    farray = source_node->children;
    while (farray != NULL) {
        if (strcmp((char*)farray->name, "float_array") == 0) {
            break;
        }
        farray = farray->next;
    }

    prop = (char*)xmlGetProp(farray, BAD_CAST "count");
    num_floats = atoi(prop);
    free(prop);

    rv = new FloatSource(num_floats);

    all_floats = (char*)xmlNodeListGetString(farray->doc, farray->children, 1);
    this_float = strtok_r(all_floats, " ", &next_float);
    i = 0;
    do {
        rv->data[i] = (float)atof(this_float);
        ++i;
        this_float = strtok_r(NULL, " ", &next_float);
    } while (this_float != NULL);
    free(all_floats);

    return rv;
}

std::string loadVertices(xmlNodePtr vertices_node) {
    xmlNodePtr curr = vertices_node->children;
    char *prop, *prop2;
    std::string rv;

    while (curr != NULL) {
        if (strcmp((char*)curr->name, "input") == 0) {
            prop = (char*)xmlGetProp(curr, BAD_CAST "semantic");
            if (strcmp(prop, "POSITION") == 0) {
                prop2 = (char*)xmlGetProp(curr, BAD_CAST "source");
                rv = std::string(prop2);
                free(prop2);
            }
            free(prop);
        }
        curr = curr->next;
    }

    if (!rv.empty()) {
        // Strip off the leading "#".
        rv.replace(0, 1, "");
    }

    return rv;
}

void loadMesh(xmlNodePtr mesh_node) {
    xmlNodePtr curr = mesh_node->children;
    std::map<std::string, FloatSource*> sources;
    std::map<std::string, FloatSource*>::const_iterator it;
    std::string name;
    FloatSource *value;
    char *prop;

    while (curr != NULL) {
        if (strcmp((char*)curr->name, "source") == 0) {
            prop = (char*)xmlGetProp(curr, BAD_CAST "id");
            sources[std::string(prop)] = loadSource(curr);
            free(prop);
        }
        curr = curr->next;
    }

    printf("map size (1) = %ld\n", sources.size());
    for (it = sources.begin(); it != sources.end(); ++it) {
        printf("(1) name = %s value (length) = %d\n", it->first.c_str(), it->second->length);
    }

    curr = mesh_node->children;
    while (curr != NULL) {
        if (strcmp((char*)curr->name, "vertices") == 0) {
            name = loadVertices(curr);
            prop = (char*)xmlGetProp(curr, BAD_CAST "id");
            value = sources[name];
            sources[std::string(prop)] = value;
            sources.erase(name);
            free(prop);
        }
        curr = curr->next;
    }

    for (it = sources.begin(); it != sources.end(); ++it) {
        printf("(2) name = %s value (length) = %d\n", it->first.c_str(), it->second->length);
        delete it->second;
    }
}

void loadGeometry(xmlNodePtr geometry_node) {
    xmlNodePtr curr = geometry_node->children;

    while (curr != NULL) {
        if (strcmp((char*)curr->name, "mesh") == 0) {
            return loadMesh(curr);
        }
        curr = curr->next;
    }
}
