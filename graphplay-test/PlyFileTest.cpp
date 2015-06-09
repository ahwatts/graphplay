// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "../graphplay/PlyFile.h"

#include <iostream>
#include <string>
#include <sstream>
#include <gtest/gtest.h>

namespace graphplay {
    TEST(PlyFileTest, ReadFormat) {
        PlyFile f;
        std::string ply_string(R"ply(ply
format ascii 1.0
)ply");
        std::istringstream ply_stream(ply_string);
        f.load(ply_stream);
        ASSERT_EQ(PlyFile::ASCII, f.getFormat());
    }

    TEST(PlyFileTest, ReadComments) {
        PlyFile f;
        std::string ply_string(R"ply(ply
comment Test comment
comment Second test comment
)ply");

        std::istringstream ply_stream(ply_string);
        f.load(ply_stream);

        const std::vector<std::string> &comments = f.getComments();
        ASSERT_EQ(2, comments.size());
        ASSERT_EQ("Test comment", comments[0]);
        ASSERT_EQ("Second test comment", comments[1]);
    }

    TEST(PlyFileTest, ReadElement) {
        PlyFile f;
        std::string ply_string(R"ply(ply
element vertex 6
element face 8
)ply");

        std::istringstream ply_stream(ply_string);
        f.load(ply_stream);

        const std::vector<PlyFile::Element> &elements = f.getElements();
        ASSERT_EQ(2, elements.size());
        ASSERT_EQ("vertex", elements[0].name);
        ASSERT_EQ(6, elements[0].count);
        ASSERT_EQ("face", elements[1].name);
        ASSERT_EQ(8, elements[1].count);
    }
}
