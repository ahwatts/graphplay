// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "PlyFile.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace graphplay::ply;

typedef std::vector<std::string> StringVec;

StringVec  split(const std::string &str, char sep);
StringVec& split(const std::string &str, char sep, StringVec &tokens);

std::string  join(StringVec::const_iterator begin, StringVec::const_iterator end, char sep);
std::string &join(StringVec::const_iterator begin, StringVec::const_iterator end, char sep, std::string &dest);

Format read_format(StringVec &toks);
std::string read_comment(StringVec &toks);
Element read_element(StringVec &toks);
Property read_property(StringVec &toks);
ValueType read_value_type(std::string &type_str);
void read_ascii_values(Element &e, std::istream &stream);
void read_scalar_property_value(std::ostream &stream, const Property &prop, const std::string &value);

// Class PlyFile.

PlyFile::PlyFile()
    : m_format(ASCII),
      m_comments(),
      m_elements()
{}

PlyFile::PlyFile(const char *filename) : PlyFile() {
    std::fstream file(filename, std::ios::in | std::ios::binary);
    load(file);
    file.close();
}

PlyFile::PlyFile(std::istream &stream) : PlyFile() {
    load(stream);
}

PlyFile::~PlyFile() {}

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

    for (std::vector<Element>::iterator e = m_elements.begin(); e != m_elements.end(); ++e) {
        switch (m_format) {
        case ASCII:
            read_ascii_values(*e, stream);
            break;
        case BINARY_BIG_ENDIAN:
            break;
        case BINARY_LITTLE_ENDIAN:
            break;
        }
    }
}

// PlyFile-specific utility functions.

Format read_format(StringVec &toks) {
    Format rv = ASCII;

    if (toks.size() >= 2) {
        if (toks[1] == "ascii") {
            rv = ASCII;
        } else if (toks[1] == "binary_big_endian") {
            rv = BINARY_BIG_ENDIAN;
        } else if (toks[1] == "binary_little_endian") {
            rv = BINARY_LITTLE_ENDIAN;
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

Element read_element(StringVec &toks) {
    Element rv;

    if (toks.size() >= 3) {
        rv.name = toks[1];
        rv.count = std::stoi(toks[2]);
    }

    return rv;
}

Property read_property(StringVec &toks) {
    Property rv;

    if (toks.size() >= 2) {
        if (toks[1] == "list") {
            rv.list = true;
        } else {
            rv.list = false;
        }
    }

    if (rv.list && toks.size() >= 5) {
        rv.name = toks[4];
        rv.count_type = read_value_type(toks[2]);
        rv.value_type = read_value_type(toks[3]);
    } else if (!rv.list && toks.size() >= 3) {
        rv.name = toks[2];
        rv.type = read_value_type(toks[1]);
    }

    return rv;
}

ValueType read_value_type(std::string &type_str) {
    if (type_str == "uint8" || type_str == "uchar") {
        return UINT_8;
    } else if (type_str == "int8" || type_str == "char") {
        return INT_8;
    } else if (type_str == "uint16" || type_str == "ushort") {
        return UINT_16;
    } else if (type_str == "int16" || type_str == "short") {
        return INT_16;
    } else if (type_str == "uint32" || type_str == "uint") {
        return UINT_32;
    } else if (type_str == "int32" || type_str == "int") {
        return INT_32;
    } else if (type_str == "float" || type_str == "float32") {
        return FLOAT_32;
    } else if (type_str == "double" || type_str == "float64") {
        return FLOAT_64;
    } else {
        // ???
        return UINT_8;
    }
}

void read_ascii_values(Element &elem, std::istream &stream) {
    std::string line;
    std::ostringstream data_stream;

    while (std::getline(stream, line)) {
        StringVec tokens = split(line, ' ');
        auto token = tokens.begin();
        auto prop = elem.props.begin();
        while (token != tokens.end() && prop != elem.props.end()) {
            if (prop->list) {
            } else {
                read_scalar_property_value(data_stream, *prop, *token);
            }

            ++token;
            ++prop;
        }
    }

    elem.data += data_stream.str();
}

void read_scalar_property_value(std::ostream &to, const Property &prop, const std::string &from) {
    union {
        std::uint8_t  uc_value;
        std::uint16_t us_value;
        std::uint32_t ui_value;

        std::int8_t  c_value;
        std::int16_t s_value;
        std::int32_t i_value;

        float  f_value;
        double d_value;
    } value;

    switch (prop.type) {
    case UINT_8:
        value.uc_value = (std::uint8_t)std::stoul(from);
        to.write((char *)&value, 1);
        break;
    case UINT_16:
        value.us_value = (std::uint16_t)std::stoul(from);
        to.write((char *)&value, 2);
        break;
    case UINT_32:
        value.ui_value = (std::uint32_t)std::stoul(from);
        to.write((char *)&value, 4);
        break;
    case INT_8:
        value.c_value = (std::int8_t)std::stol(from);
        to.write((char *)&value, 1);
        break;
    case INT_16:
        value.s_value = (std::int16_t)std::stol(from);
        to.write((char *)&value, 2);
        break;
    case INT_32:
        value.i_value = (std::int32_t)std::stol(from);
        to.write((char *)&value, 4);
        break;
    case FLOAT_32:
        value.f_value = std::stof(from);
        to.write((char *)&value, sizeof(float));
        break;
    case FLOAT_64:
        value.d_value = std::stod(from);
        to.write((char *)&value, sizeof(double));
        break;
    default:
        std::cerr << "Cannot handle PLY value type " << prop.type << " with value " << from << std::endl;
        break;
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
