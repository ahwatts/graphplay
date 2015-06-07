// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "PlyFile.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace graphplay {
    void tokenize(std::string &str, std::vector<std::string> &tokens);
    PlyFile::Format read_format(std::vector<std::string> &toks);

    // Class PlyFile.

    PlyFile::PlyFile()
        : m_format(ASCII)
          // m_comments(),
          // m_elements()
    {}

    PlyFile::~PlyFile() {}

    void PlyFile::load(const char *filename) {
        std::fstream file(filename, std::ios::in | std::ios::binary);
        load(file);
        file.close();
    }

    void PlyFile::load(std::istream &stream) {
        std::string magic, line;
        std::getline(stream, magic);
        if (magic != "ply") {
            return;
        }

        while (std::getline(stream, line)) {
            std::vector<std::string> tokens;
            tokenize(line, tokens);

            if (tokens.size() > 0) {
                if (tokens[0] == "format") {
                    m_format = read_format(tokens);
                } else if (tokens[0] == "comment") {
                } else if (tokens[0] == "element") {
                } else if (tokens[0] == "property") {
                } else if (tokens[0] == "end_header") {
                    break;
                }
            }
        }
    }

    // Utility functions.

    void tokenize(std::string &str, std::vector<std::string> &tokens) {
        std::istringstream stream(str);
        std::string token;

        while (std::getline(stream, token, ' ')) {
            tokens.emplace_back(token);
        }
    }

    PlyFile::Format read_format(std::vector<std::string> &toks) {
        PlyFile::Format rv = PlyFile::ASCII;

        if (toks.size() >= 2) {
            if (toks[1] == "ascii") {
                rv = PlyFile::ASCII;
            } else if (toks[1] == "binary_big_endian") {
                rv = PlyFile::BINARY_BIG_ENDIAN;
            } else if (toks[1] == "binary_little_endian") {
                rv = PlyFile::BINARY_LITTLE_ENDIAN;
            }
        }

        return rv;
    }
}
