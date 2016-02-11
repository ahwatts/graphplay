// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "../graphplay/PlyFile.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <sstream>
#include <gtest/gtest.h>

namespace graphplay {
    using namespace ply;

    TEST(PlyFileTest, ReadFormat) {
        std::string ply_string(R"ply(ply
format ascii 1.0
)ply");
        std::istringstream ply_stream(ply_string);
        PlyFile f(ply_stream);
        ASSERT_EQ(ASCII, f.getFormat());
    }

    TEST(PlyFileTest, ReadComments) {
        std::string ply_string(R"ply(ply
comment Test comment
comment Second test comment
)ply");

        std::istringstream ply_stream(ply_string);
        PlyFile f(ply_stream);

        const std::vector<std::string> &comments = f.getComments();
        ASSERT_EQ(2, comments.size());
        ASSERT_EQ("Test comment", comments[0]);
        ASSERT_EQ("Second test comment", comments[1]);
    }

    TEST(PlyFileTest, ReadElement) {
        std::string ply_string(R"ply(ply
element vertex 6
element face 8
)ply");

        std::istringstream ply_stream(ply_string);
        PlyFile f(ply_stream);

        const std::vector<Element> &elements = f.getElements();
        ASSERT_EQ(2, elements.size());
        ASSERT_EQ("vertex", elements[0].name);
        ASSERT_EQ(6, elements[0].count);
        ASSERT_EQ("face", elements[1].name);
        ASSERT_EQ(8, elements[1].count);
    }

    TEST(PlyFileTest, ReadProperties) {
        std::string ply_string(R"ply(ply
element vertex 6
property float32 x
element face 8
property list uint8 uint32 vertex_indices
)ply");

        std::istringstream ply_stream(ply_string);
        PlyFile f(ply_stream);

        const std::vector<Element> &elements = f.getElements();

        ASSERT_EQ(2, elements.size());
        ASSERT_EQ(1, elements[0].props.size());
        ASSERT_EQ("x", elements[0].props[0].name);
        ASSERT_EQ(false, elements[0].props[0].list);
        ASSERT_EQ(ValueType::FLOAT_32, elements[0].props[0].type);

        ASSERT_EQ(1, elements[1].props.size());
        ASSERT_EQ("vertex_indices", elements[1].props[0].name);
        ASSERT_EQ(true, elements[1].props[0].list);
        ASSERT_EQ(ValueType::UINT_8, elements[1].props[0].count_type);
        ASSERT_EQ(ValueType::UINT_32, elements[1].props[0].value_type);
    }

    TEST(PlyFileTest, ReadScalarAsciiData) {
        std::string ply_string(R"ply(ply
format ascii 1.0
element vertex 2
property uint8 uc
property uint16 us
property uint32 ul
property int8 c
property int16 s
property int32 l
property float32 f
property float64 d
end_header
1 2 3 -1 -2 -3 1.0 2.3
2 4 6 -2 -4 -6 2.1 4.0
)ply");

        std::istringstream ply_stream(ply_string);
        PlyFile f(ply_stream);

        const std::vector<Element> &elements = f.getElements();

        ASSERT_EQ(1, elements.size());
        ASSERT_EQ(8, elements[0].props.size());

        const Element &elem = elements[0];

#ifdef _MSC_VER
#pragma pack(1)
#endif
        struct
#ifdef __GNUC__
            [[gnu::packed]]
#endif
        {
            std::uint8_t  uc;
            std::uint16_t us;
            std::uint32_t ui;
            std::int8_t   c;
            std::int16_t  s;
            std::int32_t  i;
            float  f;
            double d;
        } value;
        ASSERT_EQ(2*sizeof(value), elem.data.size());

        std::istringstream data_stream(elem.data);

        data_stream.read((char*)&value, sizeof(value));
        ASSERT_EQ(1, value.uc);
        ASSERT_EQ(2, value.us);
        ASSERT_EQ(3, value.ui);
        ASSERT_EQ(-1, value.c);
        ASSERT_EQ(-2, value.s);
        ASSERT_EQ(-3, value.i);
        ASSERT_FLOAT_EQ(1.0, value.f);
        ASSERT_DOUBLE_EQ(2.3, value.d);

        data_stream.read((char*)&value, sizeof(value));
        ASSERT_EQ(2, value.uc);
        ASSERT_EQ(4, value.us);
        ASSERT_EQ(6, value.ui);
        ASSERT_EQ(-2, value.c);
        ASSERT_EQ(-4, value.s);
        ASSERT_EQ(-6, value.i);
        ASSERT_FLOAT_EQ(2.1f, value.f);
        ASSERT_DOUBLE_EQ(4.0, value.d);
    }
}
