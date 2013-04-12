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

void tokenizeStringToFloatArray(float *dest, char *all_floats) {
    char *this_float = NULL, *next_float = NULL;
    int i = 0;

    this_float = strtok_r(all_floats, " ", &next_float);
    do {
        dest[i] = (float)atof(this_float);
        ++i;
        this_float = strtok_r(NULL, " ", &next_float);
    } while (this_float != NULL);
}

void tokenizeStringToIntArray(int *dest, char *all_ints) {
    char *this_int = NULL, *next_int = NULL;
    int i = 0;

    this_int = strtok_r(all_ints, " ", &next_int);
    do {
        dest[i] = (int)atoi(this_int);
        ++i;
        this_int = strtok_r(NULL, " ", &next_int);
    } while (this_int != NULL);
}

FloatSource *loadSource(xmlNodePtr source_node) {
    FloatSource *rv = NULL;
    int num_floats;
    xmlNodePtr farray;
    char *all_floats, *prop;

    farray = source_node->children;
    while (farray != NULL) {
        if (strcmp((char*)farray->name, "float_array") == 0) {
            break;
        }
        farray = farray->next;
    }

    if (farray == NULL) {
        return NULL;
    }

    prop = (char*)xmlGetProp(farray, BAD_CAST "count");
    num_floats = atoi(prop);
    xmlFree(prop);

    rv = new FloatSource(num_floats);

    all_floats = (char*)xmlNodeListGetString(farray->doc, farray->children, 1);
    tokenizeStringToFloatArray(rv->data, all_floats);
    xmlFree(all_floats);

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
                xmlFree(prop2);
            }
            xmlFree(prop);
        }
        curr = curr->next;
    }

    if (!rv.empty()) {
        // Strip off the leading "#".
        rv.replace(0, 1, "");
    }

    return rv;
}

FloatSource *loadPolyList(xmlNodePtr pl_node, std::map<std::string, FloatSource*> &sources) {
    xmlNodePtr curr;
    char *prop, *prop2, *vcounts_str, *ps_str;
    int num_polys, num_verts, attrs_per_vert, vals_per_attr = 3, *vcounts, *ps;
    std::map<int, std::string> inputs;
    std::map<int, std::string>::const_iterator it;
    FloatSource *rv;

    // How many polys are there?
    prop = (char*)xmlGetProp(pl_node, BAD_CAST "count");
    num_polys = atoi(prop);
    xmlFree(prop);

    printf("There are %d polygons.\n", num_polys);

    // Figure out which index is which.
    curr = pl_node->children;
    while (curr != NULL) {
        if (strcmp((char*)curr->name, "input") == 0) {
            prop = (char*)xmlGetProp(curr, BAD_CAST "source");
            prop2 = (char*)xmlGetProp(curr, BAD_CAST "offset");
            inputs[atoi(prop2)] = std::string(prop);
            inputs[atoi(prop2)].replace(0, 1, "");
            xmlFree(prop);
            xmlFree(prop2);
        } else if (strcmp((char*)curr->name, "vcount") == 0) {
            vcounts_str = (char*)xmlNodeListGetString(curr->doc, curr->children, 1);
        } else if (strcmp((char*)curr->name, "p") == 0) {
            ps_str = (char*)xmlNodeListGetString(curr->doc, curr->children, 1);
        }
        curr = curr->next;
    }

    // Bail if there are problems.
    if (!(vcounts_str && ps_str && num_polys > 0)) {
        return NULL;
    }

    attrs_per_vert = inputs.size();
    vals_per_attr = 3;

    // Extract the vertex counts; count all the vertices.
    vcounts = new int[num_polys];
    tokenizeStringToIntArray(vcounts, vcounts_str);
    num_verts = 0;
    for (int i = 0; i < num_polys; ++i) {
        num_verts += vcounts[i];
    }

    // Extract the source indexes.
    ps = new int[num_verts*attrs_per_vert];
    tokenizeStringToIntArray(ps, ps_str);

    // Initialize our return value.
    rv = new FloatSource(num_verts*attrs_per_vert*vals_per_attr);

    // Copy the data from the sources into the return value.
    for (int i = 0; i < num_verts; ++i) {
        for (it = inputs.begin(); it != inputs.end(); ++it) {
            const int &p_offset = it->first;
            const std::string &source_name = it->second;

            int source_index = ps[i*attrs_per_vert + p_offset];
            int dest_index = i*attrs_per_vert*vals_per_attr + p_offset*vals_per_attr;
            FloatSource *source = sources[source_name];

            rv->data[dest_index+0] = source->data[source_index+0];
            rv->data[dest_index+1] = source->data[source_index+1];
            rv->data[dest_index+2] = source->data[source_index+2];
        }
    }

    /* for (int i = 0; i < num_verts; ++i) {
        int j = i*attrs_per_vert*vals_per_attr;
        printf("vertex[%2d] = (%6.3f, %6.3f, %6.3f) (%6.3f, %6.3f, %6.3f)\n", i,
               rv->data[j+0], rv->data[j+1], rv->data[j+2],
               rv->data[j+3], rv->data[j+4], rv->data[j+5]);
       } */

    // Clean up.
    delete [] ps;
    delete [] vcounts;
    xmlFree(vcounts_str);
    xmlFree(ps_str);

    return rv;
}

void loadMesh(xmlNodePtr mesh_node) {
    xmlNodePtr curr;
    std::map<std::string, FloatSource*> sources;
    std::map<std::string, FloatSource*>::const_iterator it;
    std::string name;
    FloatSource *value;
    char *prop;

    // Read the "source" elements to get the raw data.
    curr = mesh_node->children;
    while (curr != NULL) {
        if (strcmp((char*)curr->name, "source") == 0) {
            prop = (char*)xmlGetProp(curr, BAD_CAST "id");
            sources[std::string(prop)] = loadSource(curr);
            xmlFree(prop);
        }
        curr = curr->next;
    }

    // The "vertices" element seems just to forward to the source
    // element; update it in the hash.
    curr = mesh_node->children;
    while (curr != NULL) {
        if (strcmp((char*)curr->name, "vertices") == 0) {
            name = loadVertices(curr);
            prop = (char*)xmlGetProp(curr, BAD_CAST "id");
            value = sources[name];
            sources[std::string(prop)] = value;
            sources.erase(name);
            xmlFree(prop);
        }
        curr = curr->next;
    }

    // Read the polylist and assemble the data.
    curr = mesh_node->children;
    while (curr != NULL) {
        if (strcmp((char*)curr->name, "polylist") == 0) {
            loadPolyList(curr, sources);
        }
        curr = curr->next;
    }

    // Clean up.
    for (it = sources.begin(); it != sources.end(); ++it) {
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
