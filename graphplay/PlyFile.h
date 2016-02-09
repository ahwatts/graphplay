// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_PLY_FILE_H_
#define _GRAPHPLAY_GRAPHPLAY_PLY_FILE_H_

#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include <iostream>

namespace graphplay {
    class PlyFile {
    public:
        enum Format {
            ASCII,
            BINARY_BIG_ENDIAN,
            BINARY_LITTLE_ENDIAN,
        };

        enum ValueType {
            INT_8,  UINT_8,
            INT_16, UINT_16,
            INT_32, UINT_32,
            FLOAT_32,
            FLOAT_64,
        };

        struct Property {
            std::string name;
            bool list;
            union {
                ValueType type;
                struct {
                    ValueType count_type, value_type;
                };
            };
        };

        struct Element {
            std::string name;
            int count;
            std::vector<Property> props;
            std::string data;
        };

        PlyFile();
        PlyFile(const char *filename);
        PlyFile(std::istream &stream);
        PlyFile(const PlyFile &other) = delete;
        PlyFile(PlyFile &&other) = delete;
        ~PlyFile();

        PlyFile& operator=(const PlyFile &other) = delete;
        PlyFile& operator=(PlyFile &&other) = delete;

        inline Format getFormat() const { return m_format; };
        inline const std::vector<std::string>& getComments() const { return m_comments; };
        inline const std::vector<Element>& getElements() const { return m_elements; };

    private:
        void load(std::istream &stream);

        Format m_format;
        std::vector<std::string> m_comments;
        std::vector<Element> m_elements;
    };
}

#endif
