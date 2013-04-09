#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "DaeFile.h"

DaeFile* DaeFile::loadDaeFile(const char* filename) {
    xmlDocPtr doc = NULL;
    xmlNodePtr node = NULL;

    doc = xmlParseFile(filename);
    if (doc == NULL) {
        fprintf(stderr, "Failed to read XML file %s\n", filename);
        exit(1);
    }

    xmlFreeDoc(doc);

    return NULL;
}

DaeFile::DaeFile(void)
{
}


DaeFile::~DaeFile(void)
{
}
