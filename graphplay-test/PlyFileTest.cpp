// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "../graphplay/PlyFile.h"

#include <iostream>
#include <string>
#include <sstream>
#include <gtest/gtest.h>

namespace graphplay {
    TEST(PlyFileTest, ReadFormat) {
        std::string ply_string(R"ply(ply
format ascii 1.0
)ply");
        std::istringstream ply_stream(ply_string);
        PlyFile f(ply_stream);
        ASSERT_EQ(PlyFile::ASCII, f.getFormat());
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

        const std::vector<PlyFile::Element> &elements = f.getElements();
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

        const std::vector<PlyFile::Element> &elements = f.getElements();

        ASSERT_EQ(2, elements.size());
        ASSERT_EQ(1, elements[0].props.size());
        ASSERT_EQ("x", elements[0].props[0].name);
        ASSERT_EQ(false, elements[0].props[0].list);
        ASSERT_EQ(PlyFile::ValueType::FLOAT_32, elements[0].props[0].type);

        ASSERT_EQ(1, elements[1].props.size());
        ASSERT_EQ("vertex_indices", elements[1].props[0].name);
        ASSERT_EQ(true, elements[1].props[0].list);
        ASSERT_EQ(PlyFile::ValueType::UINT_8, elements[1].props[0].count_type);
        ASSERT_EQ(PlyFile::ValueType::UINT_32, elements[1].props[0].value_type);
    }
}
