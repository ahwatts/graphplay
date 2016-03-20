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

        ASSERT_EQ(1, data[0].getProperty("uc").first<int>());
        ASSERT_EQ(2, data[0].getProperty("us").first<int>());
        ASSERT_EQ(3, data[0].getProperty("ul").first<int>());
        ASSERT_EQ(-1, data[0].getProperty("c").first<int>());
        ASSERT_EQ(-2, data[0].getProperty("s").first<int>());
        ASSERT_EQ(-3, data[0].getProperty("l").first<int>());
        ASSERT_FLOAT_EQ(1.0, data[0].getProperty("f").first<float>());
        ASSERT_FLOAT_EQ(2.3, data[0].getProperty("d").first<float>());

        ASSERT_EQ(2, data[1].getProperty("uc").first<int>());
        ASSERT_EQ(4, data[1].getProperty("us").first<int>());
        ASSERT_EQ(6, data[1].getProperty("ul").first<int>());
        ASSERT_EQ(-2, data[1].getProperty("c").first<int>());
        ASSERT_EQ(-4, data[1].getProperty("s").first<int>());
        ASSERT_EQ(-6, data[1].getProperty("l").first<int>());
        ASSERT_FLOAT_EQ(2.1, data[1].getProperty("f").first<float>());
        ASSERT_FLOAT_EQ(4.0, data[1].getProperty("d").first<float>());
    }

    TEST(PlyFileTest, ReadListAsciiData) {
        std::string ply_string(R"ply(ply
format ascii 1.0
element vertex 2
property list uint8 uint8 ucl
property list uint16 int32 ll
property list uint16 float32 fl
end_header
3 1 2 3 3 -2 -4 -6 4 1.0 2.1 3.2 4.3
)ply");

        std::istringstream ply_stream(ply_string);
        PlyFile f(ply_stream);

        const std::vector<Element> &elements = f.elements();
        ASSERT_EQ(1, elements.size());

        const Element &elem = elements[0];
        ASSERT_EQ(3, elem.properties().size());

        const std::vector<ElementValue> &data = elem.data();
        ASSERT_EQ(1, data.size());

        const PropertyValue &ucl_val = data[0].getProperty("ucl");
        ASSERT_EQ(3, ucl_val.size());
        ASSERT_TRUE(ucl_val.isList());
        ASSERT_TRUE(ucl_val.isIntegral());

        PropertyValueIterator<unsigned char> i = ucl_val.begin<unsigned char>();
        ASSERT_EQ(1, *i++);
        ASSERT_EQ(2, *i++);
        ASSERT_EQ(3, *i++);
        ASSERT_EQ(ucl_val.end<unsigned char>(), i);

        const PropertyValue &ll_val = data[0].getProperty("ll");
        ASSERT_EQ(3, ll_val.size());
        ASSERT_TRUE(ll_val.isList());
        ASSERT_TRUE(ll_val.isIntegral());

        PropertyValueIterator<long> j = ll_val.begin<long>();
        ASSERT_EQ(-2, *j++);
        ASSERT_EQ(-4, *j++);
        ASSERT_EQ(-6, *j++);
        ASSERT_EQ(ll_val.end<long>(), j);

        const PropertyValue &fl_val = data[0].getProperty("fl");
        ASSERT_EQ(4, fl_val.size());
        ASSERT_TRUE(fl_val.isList());
        ASSERT_FALSE(fl_val.isIntegral());

        PropertyValueIterator<float> k = fl_val.begin<float>();
        ASSERT_FLOAT_EQ(1.0, *k++);
        ASSERT_FLOAT_EQ(2.1, *k++);
        ASSERT_FLOAT_EQ(3.2, *k++);
        ASSERT_FLOAT_EQ(4.3, *k++);
        ASSERT_EQ(fl_val.end<float>(), k);
    }
}
