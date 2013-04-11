// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <map>
#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "DaeFile.h"

#ifdef _WIN32
#define snprintf _snprintf_s
#endif

xmlXPathContextPtr createDaeFileXPathContext(xmlDocPtr doc);
void loadGeometry(xmlNodePtr node);

DaeFile* DaeFile::loadDaeFile(const char* filename) {
    xmlDocPtr doc = NULL;
    xmlNodePtr node = NULL;
    xmlXPathContextPtr xpath_ctxt = NULL;
    xmlXPathObjectPtr xpath_result = NULL;
    int i;

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
        for (i = 0; i < xmlXPathNodeSetGetLength(xpath_result->nodesetval); ++i) {
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

DaeFile::DaeFile(void)
{
}


DaeFile::~DaeFile(void)
{
}

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
    FloatSource(int length);
    ~FloatSource();

    float *data;
    int length;
};

FloatSource::FloatSource(int length)
    : length(length),
      data(new float[length])
{ }

FloatSource::~FloatSource() {
    if (data) {
        delete data;
    }
}

FloatSource *loadSource(xmlNodePtr source_node) {
    FloatSource *rv = NULL;
    int num_floats, i;
    xmlNodePtr farray;
    char *all_floats, *this_float, *next_float;

    // We're going to ignore the accessor element and assume that the data
    // is in blocks of 3 going like x, y, z.
    farray = source_node->children;
    while (farray != NULL) {
        if (strcmp((char*)farray->name, "float_array") == 0) {
            break;
        }
        farray = farray->next;
    }

    num_floats = atoi((char*)xmlGetProp(farray, BAD_CAST "count"));
    rv = new FloatSource(num_floats);

    all_floats = (char*)xmlNodeListGetString(farray->doc, farray->children, 1);
    all_floats = _strdup(all_floats);
    this_float = strtok_s(all_floats, " ", &next_float);
    i = 0;
    do {
        rv->data[i] = (float)atof(this_float);
        ++i;
        this_float = strtok_s(NULL, " ", &next_float);
    } while (this_float != NULL);

    free(all_floats);

    return rv;
}

void loadMesh(xmlNodePtr mesh_node) {
    xmlNodePtr curr = mesh_node->children;
    std::map<char*, FloatSource*> sources;
    std::map<char*, FloatSource*>::iterator it;
    char *name;
    FloatSource *value;

    while (curr != NULL) {
        if (strcmp((char*)curr->name, "source") == 0) {
            sources[(char*)xmlGetProp(curr, BAD_CAST "id")] = loadSource(curr);
        }
        curr = curr->next;
    }

    for (it = sources.begin(); it != sources.end(); it++) {
        value = it->second;
        sources.erase(it);
        if (value) {
            delete value;
        }
    }
    sources.clear();
}

void loadGeometry(xmlNodePtr geometry_node) {
    xmlNodePtr curr = geometry_node->children, polylist = NULL;
    int num_polys = 0;

    while (curr != NULL) {
        if (strcmp((char*)curr->name, "mesh") == 0) {
            return loadMesh(curr);
        }
        curr = curr->next;
    }
}
