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

        ASSERT_EQ(2, f.comments_size());

        PlyFile::const_comment_iterator c = f.cbegin_comments();
        ASSERT_EQ("Test comment", *c++);
        ASSERT_EQ("Second test comment", *c++);
        ASSERT_EQ(f.cend_comments(), c);
    }

    TEST(PlyFileTest, ReadElement) {
        std::string ply_string(R"ply(ply
element vertex 6
element face 8
)ply");

        std::istringstream ply_stream(ply_string);
        PlyFile f(ply_stream);

        ASSERT_EQ(2, f.elements_size());

        PlyFile::const_element_iterator e = f.cbegin_elements();
        ASSERT_EQ("vertex", e->name());
        ASSERT_EQ(6, e->count());

        ++e;

        ASSERT_EQ("face", e->name());
        ASSERT_EQ(8, e->count());

        ++e;

        ASSERT_EQ(f.cend_elements(), e);
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

        ASSERT_EQ(2, f.elements_size());

        PlyFile::const_element_iterator e = f.cbegin_elements();
        ASSERT_EQ(1, e->properties().size());
        ASSERT_EQ("x", e->properties()[0].name());
        ASSERT_EQ(false, e->properties()[0].isList());
        ASSERT_EQ(false, e->properties()[0].isIntegral());

        ++e;

        ASSERT_EQ(1, e->properties().size());
        ASSERT_EQ("vertex_indices", e->properties()[0].name());
        ASSERT_EQ(true, e->properties()[0].isList());
        ASSERT_EQ(true, e->properties()[0].isIntegral());

        ++e;

        ASSERT_EQ(f.cend_elements(), e);
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

        ASSERT_EQ(1, f.elements_size());

        PlyFile::const_element_iterator elem_iter = f.cbegin_elements();
        const Element &elem = *elem_iter;
        ASSERT_EQ(8, elem.properties().size());

        const std::vector<ElementValue> &data = elem.data();
        ASSERT_EQ(2, data.size());

        ASSERT_EQ(1, data[0].getProperty("uc").first<int>());
        ASSERT_EQ(2, data[0].getProperty("us").first<int>());
        ASSERT_EQ(3, data[0].getProperty("ul").first<int>());
        ASSERT_EQ(-1, data[0].getProperty("c").first<int>());
        ASSERT_EQ(-2, data[0].getProperty("s").first<int>());
        ASSERT_EQ(-3, data[0].getProperty("l").first<int>());
        ASSERT_FLOAT_EQ(1.0f, data[0].getProperty("f").first<float>());
        ASSERT_FLOAT_EQ(2.3f, data[0].getProperty("d").first<float>());

        ASSERT_EQ(2, data[1].getProperty("uc").first<int>());
        ASSERT_EQ(4, data[1].getProperty("us").first<int>());
        ASSERT_EQ(6, data[1].getProperty("ul").first<int>());
        ASSERT_EQ(-2, data[1].getProperty("c").first<int>());
        ASSERT_EQ(-4, data[1].getProperty("s").first<int>());
        ASSERT_EQ(-6, data[1].getProperty("l").first<int>());
        ASSERT_FLOAT_EQ(2.1f, data[1].getProperty("f").first<float>());
        ASSERT_FLOAT_EQ(4.0f, data[1].getProperty("d").first<float>());

        ++elem_iter;
        ASSERT_EQ(f.cend_elements(), elem_iter);
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

        ASSERT_EQ(1, f.elements_size());

        PlyFile::const_element_iterator elem_iter = f.cbegin_elements();
        const Element &elem = *elem_iter;
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
        ASSERT_FLOAT_EQ(1.0f, *k++);
        ASSERT_FLOAT_EQ(2.1f, *k++);
        ASSERT_FLOAT_EQ(3.2f, *k++);
        ASSERT_FLOAT_EQ(4.3f, *k++);
        ASSERT_EQ(fl_val.end<float>(), k);

        ++elem_iter;
        ASSERT_EQ(f.cend_elements(), elem_iter);
    }
}
