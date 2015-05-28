// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "PlyFile.h"

#include <fstream>

namespace graphplay {
    PlyFile::PlyFile()
        : m_comments(),
          m_elements()
    {}

    PlyFile::~PlyFile() {}

    void PlyFile::load(const char *filename) {
        std::fstream file(filename, std::ios::in | std::ios::binary);
        load(file);
        file.close();
    }

    void PlyFile::load(std::istream &stream) {
        
    }
}
