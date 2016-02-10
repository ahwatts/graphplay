// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "PlyFile.h"

#include <cstdint>
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
    PlyFile::ValueType read_value_type(std::string &type_str);
    void read_ascii_values(PlyFile::Element &e, std::istream &stream);

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

        for (std::vector<PlyFile::Element>::iterator e = m_elements.begin(); e != m_elements.end(); ++e) {
            switch (m_format) {
            case PlyFile::ASCII:
                read_ascii_values(*e, stream);
                break;
            case PlyFile::BINARY_BIG_ENDIAN:
                break;
            case PlyFile::BINARY_LITTLE_ENDIAN:
                break;
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
            rv.count_type = read_value_type(toks[2]);
            rv.value_type = read_value_type(toks[3]);
        } else if (!rv.list && toks.size() >= 3) {
            rv.name = toks[2];
            rv.type = read_value_type(toks[1]);
        }

        return rv;
    }

    PlyFile::ValueType read_value_type(std::string &type_str) {
        if (type_str == "uint8" || type_str == "uchar") {
            return PlyFile::ValueType::UINT_8;
        } else if (type_str == "int8" || type_str == "char") {
            return PlyFile::ValueType::INT_8;
        } else if (type_str == "uint16" || type_str == "ushort") {
            return PlyFile::ValueType::UINT_16;
        } else if (type_str == "int16" || type_str == "short") {
            return PlyFile::ValueType::INT_16;
        } else if (type_str == "uint32" || type_str == "uint") {
            return PlyFile::ValueType::UINT_32;
        } else if (type_str == "int32" || type_str == "int") {
            return PlyFile::ValueType::INT_32;
        } else if (type_str == "float" || type_str == "float32") {
            return PlyFile::ValueType::FLOAT_32;
        } else if (type_str == "double" || type_str == "float64") {
            return PlyFile::ValueType::FLOAT_64;
        } else {
            // ???
            return PlyFile::ValueType::UINT_8;
        }
    }

    void read_ascii_values(PlyFile::Element &elem, std::istream &stream) {
        std::string line;
        std::ostringstream data_stream;
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

        while (std::getline(stream, line)) {
            StringVec tokens = split(line, ' ');
            auto token = tokens.begin();
            auto prop = elem.props.begin();
            while (token != tokens.end() && prop != elem.props.end()) {
                if (prop->list) {
                } else {
                    switch (prop->type) {
                    case PlyFile::ValueType::UINT_8:
                        value.uc_value = (std::uint8_t)std::stoul(*token);
                        data_stream.write((char *)&value, 1);
                        break;
                    case PlyFile::ValueType::UINT_16:
                        value.us_value = (std::uint16_t)std::stoul(*token);
                        data_stream.write((char *)&value, 2);
                        break;
                    case PlyFile::ValueType::UINT_32:
                        value.ui_value = (std::uint32_t)std::stoul(*token);
                        data_stream.write((char *)&value, 4);
                        break;
                    case PlyFile::ValueType::INT_8:
                        value.c_value = (std::int8_t)std::stol(*token);
                        data_stream.write((char *)&value, 1);
                        break;
                    case PlyFile::ValueType::INT_16:
                        value.s_value = (std::int16_t)std::stol(*token);
                        data_stream.write((char *)&value, 2);
                        break;
                    case PlyFile::ValueType::INT_32:
                        value.i_value = (std::int32_t)std::stol(*token);
                        data_stream.write((char *)&value, 4);
                        break;
                    case PlyFile::ValueType::FLOAT_32:
                        value.f_value = std::stof(*token);
                        data_stream.write((char *)&value, sizeof(float));
                        break;
                    case PlyFile::ValueType::FLOAT_64:
                        value.d_value = std::stod(*token);
                        data_stream.write((char *)&value, sizeof(double));
                        break;
                    default:
                        std::cerr << "Cannot handle PLY value type " << prop->type << " with value " << *token << std::endl;
                        break;
                    }
                }

                ++token;
                ++prop;
            }
        }

        elem.data += data_stream.str();
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
