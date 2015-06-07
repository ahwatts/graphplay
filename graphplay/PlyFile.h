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

        // enum PropertyType {
        //     INT_8,  UINT_8,
        //     INT_16, UINT_16,
        //     INT_32, UINT_32,
        //     FLOAT_32,
        //     FLOAT_64,
        // };

        // struct PropertyDesc {
        //     bool list;
        //     union {
        //         PropertyType type;
        //         struct {
        //             PropertyType count_type, value_type;
        //         };
        //     };
        // };

        // union Value {
        //     std::int8_t char_val;
        //     std::uint8_t uchar_val;
        //     std::int16_t short_val;
        //     std::uint16_t ushort_val;
        //     std::int32_t int_val;
        //     std::uint32_t uint_val;
        //     float float_val;
        //     double double_val;
        // };

        // typedef std::map<std::string, PropertyDesc> prop_desc_map;
        // typedef std::map<std::string, Value> prop_val_map;

        // struct Element {
        //     std::string name;
        //     prop_desc_map props;
        //     std::vector<prop_val_map> values;
        // };

        PlyFile();
        PlyFile(const char *filename);
        PlyFile(const PlyFile &other);
        PlyFile(PlyFile &&other);
        ~PlyFile();

        PlyFile& operator=(const PlyFile &other);
        PlyFile& operator=(PlyFile &&other);

        void load(const char *filename);
        void load(std::istream &stream);

        inline Format getFormat() const { return m_format; };

    private:
        Format m_format;
        std::vector<std::string> m_comments;
        // std::map<std::string, Element> m_elements;
    };
}

#endif
