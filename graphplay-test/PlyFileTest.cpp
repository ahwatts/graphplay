// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "../graphplay/PlyFile.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <sstream>
#include <gtest/gtest.h>

namespace graphplay {

    TEST(PlyFileTest, ReadComments) {
        std::string ply_string(R"ply(ply
comment Test comment
comment Second test comment
)ply");

        std::istringstream ply_stream(ply_string);
        PlyFile f(ply_stream);

        const std::vector<std::string> &comments = f.comments();
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

        const std::vector<Element> &elements = f.elements();
        ASSERT_EQ(2, elements.size());
        ASSERT_EQ("vertex", elements[0].name());
        ASSERT_EQ(6, elements[0].count());
        ASSERT_EQ("face", elements[1].name());
        ASSERT_EQ(8, elements[1].count());
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

        const std::vector<Element> &elements = f.elements();

        ASSERT_EQ(2, elements.size());
        ASSERT_EQ(1, elements[0].properties().size());
        ASSERT_EQ("x", elements[0].properties()[0].name());
        ASSERT_EQ(false, elements[0].properties()[0].isList());
        ASSERT_EQ(false, elements[0].properties()[0].isIntegral());

        ASSERT_EQ(1, elements[1].properties().size());
        ASSERT_EQ("vertex_indices", elements[1].properties()[0].name());
        ASSERT_EQ(true, elements[1].properties()[0].isList());
        ASSERT_EQ(true, elements[1].properties()[0].isIntegral());
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

        const std::vector<Element> &elements = f.elements();
        ASSERT_EQ(1, elements.size());

        const Element &elem = elements[0];
        ASSERT_EQ(8, elem.properties().size());

        const std::vector<ElementValue> &data = elem.data();
        ASSERT_EQ(2, data.size());
    }
}
