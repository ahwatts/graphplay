// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "DaeFile.h"

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
    xpath_ctxt = xmlXPathNewContext(doc);
    if (xpath_ctxt == NULL) {
        fprintf(stderr, "Failed to create XPath context!\n");
    }

    // Register a prefix for the default namespace (which doesn't have one, at
    // least in the document I have...) so that XPath can actually find the nodes.
    xmlXPathRegisterNs(xpath_ctxt, BAD_CAST "dae", doc->xmlRootNode->ns->href);

    // Do the XPath search.
    xpath_result = xmlXPathEvalExpression(BAD_CAST "//dae:geometry", xpath_ctxt);

    if (xpath_result && !xmlXPathNodeSetIsEmpty(xpath_result->nodesetval)) {
        printf("Found %d nodes!\n", xmlXPathNodeSetGetLength(xpath_result->nodesetval));
        for (i = 0; i < xmlXPathNodeSetGetLength(xpath_result->nodesetval); ++i) {
            node = xpath_result->nodesetval->nodeTab[i];
            printf("found node: name = %s\n", node->name);
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
