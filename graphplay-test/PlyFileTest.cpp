// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "../graphplay/PlyFile.h"

#include <iostream>
#include <string>
#include <sstream>
#include <gtest/gtest.h>

namespace graphplay {
    TEST(PlyFileTest, LoadString) {
        PlyFile f;
        std::string ply_string(R"ply(ply
format ascii 1.0
)ply");
        std::istringstream ply_stream(ply_string);
        f.load(ply_stream);

        ASSERT_EQ(PlyFile::ASCII, f.getFormat());
    }
}
