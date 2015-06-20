// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "PlyFile.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace graphplay {
    typedef std::vector<std::string> StringVec;

    StringVec  split(const std::string &str, char sep);
    StringVec& split(const std::string &str, char sep, StringVec &tokens);

    std::string  join(StringVec::const_iterator begin, StringVec::const_iterator end, char sep);
    std::string &join(StringVec::const_iterator begin, StringVec::const_iterator end, char sep, std::string &dest);

    PlyFile::Format read_format(StringVec &toks);
    std::string read_comment(StringVec &toks);
    PlyFile::Element read_element(StringVec &toks);
    PlyFile::Property read_property(StringVec &toks);
    PlyFile::PropertyType read_property_type(std::string &type_str);

    // Class PlyFile.

    PlyFile::PlyFile()
        : m_format(ASCII),
          m_comments(),
          m_elements()
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
            StringVec tokens = split(line, ' ');

            if (tokens.size() > 0) {
                if (tokens[0] == "format") {
                    m_format = read_format(tokens);
                } else if (tokens[0] == "comment") {
                    m_comments.emplace_back(read_comment(tokens));
                } else if (tokens[0] == "element") {
                    m_elements.emplace_back(read_element(tokens));
                } else if (tokens[0] == "property") {
                    if (!m_elements.empty()) {
                        Element &elem = m_elements.back();
                        elem.props.emplace_back(read_property(tokens));
                    }
                } else if (tokens[0] == "end_header") {
                    break;
                }
            }
        }
    }

    // PlyFile-specific utility functions.

    PlyFile::Format read_format(StringVec &toks) {
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

    std::string read_comment(StringVec &toks) {
        if (toks.size() >= 2) {
            return join(std::next(toks.cbegin()), toks.cend(), ' ');
        } else {
            return "";
        }
    }

    PlyFile::Element read_element(StringVec &toks) {
        PlyFile::Element rv;

        if (toks.size() >= 3) {
            rv.name = toks[1];
            rv.count = std::stoi(toks[2]);
        }

        return rv;
    }

    PlyFile::Property read_property(StringVec &toks) {
        PlyFile::Property rv;

        if (toks.size() >= 2) {
            if (toks[1] == "list") {
                rv.list = true;
            } else {
                rv.list = false;
            }
        }

        if (rv.list && toks.size() >= 5) {
            rv.name = toks[4];
            rv.count_type = read_property_type(toks[2]);
            rv.value_type = read_property_type(toks[3]);
        } else if (!rv.list && toks.size() >= 3) {
            rv.name = toks[2];
            rv.type = read_property_type(toks[1]);
        }

        return rv;
    }

    PlyFile::PropertyType read_property_type(std::string &type_str) {
        if (type_str == "uint8" || type_str == "uchar") {
            return PlyFile::PropertyType::UINT_8;
        } else if (type_str == "int8" || type_str == "char") {
            return PlyFile::PropertyType::INT_8;
        } else if (type_str == "uint16" || type_str == "ushort") {
            return PlyFile::PropertyType::UINT_16;
        } else if (type_str == "int16" || type_str == "short") {
            return PlyFile::PropertyType::INT_16;
        } else if (type_str == "uint32" || type_str == "uint") {
            return PlyFile::PropertyType::UINT_32;
        } else if (type_str == "int32" || type_str == "int") {
            return PlyFile::PropertyType::INT_32;
        } else if (type_str == "float" || type_str == "float32") {
            return PlyFile::PropertyType::FLOAT_32;
        } else if (type_str == "double" || type_str == "float64") {
            return PlyFile::PropertyType::FLOAT_64;
        } else {
            // ???
            return PlyFile::PropertyType::UINT_8;
        }
    }

    // Utility functions.

    StringVec split(const std::string &str, char sep) {
        StringVec tokens;
        split(str, sep, tokens);
        return tokens;
    }

    StringVec& split(const std::string &str, char sep, StringVec &tokens) {
        std::istringstream stream(str);
        std::string token;

        while (std::getline(stream, token, sep)) {
            tokens.emplace_back(token);
        }

        return tokens;
    }

    std::string &join(StringVec::const_iterator begin, StringVec::const_iterator end, char sep, std::string &dest) {
        dest.append(join(begin, end, sep));
        return dest;
    }

    std::string join(StringVec::const_iterator begin, StringVec::const_iterator end, char sep) {
        std::ostringstream stream;

        for (auto iter = begin; iter != end; ++iter) {
            stream << *iter;
            if (std::next(iter) != end) {
                stream << sep;
            }
        }

        return stream.str();
    }
}
