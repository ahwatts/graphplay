// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_LOAD_PLY_FILE_H_
#define _GRAPHPLAY_GRAPHPLAY_LOAD_PLY_FILE_H_

#include "../graphplay.h"

#include <cstddef>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace graphplay {
    enum Format {
        ASCII,
        BINARY_BIG_ENDIAN,
        BINARY_LITTLE_ENDIAN,
    };

    enum ScalarType {
        INT_8, UINT_8,
        INT_16, UINT_16,
        INT_32, UINT_32,
        FLOAT_32,
        FLOAT_64,
    };

    struct ListType {
        ScalarType count_type, value_type;
    };

    class PlyFile;
    class Element;
    class ElementValue;
    class Property;
    class PropertyValue;

    class Property {
    public:
        Property();
        Property(const char *name, ScalarType type);
        Property(const char *name, ListType type);
        Property(const Property &other);
        Property(Property &&other);
        ~Property();

        Property& operator=(const Property &other);
        Property& operator=(Property &&other);

        const std::string& name() const;
        const char* name_c() const;
        bool isList() const;
        bool isIntegral() const;

        friend class Element;

    private:
        class Type;

        std::string m_name;
        std::unique_ptr<Type> m_type;
    };

    template<typename T>
    class PropertyValueIterator : public std::iterator<std::forward_iterator_tag, T> {
        static_assert(std::is_arithmetic<T>::value, "Can only cast PropertyValues to arithmetic types.");

    public:
        typedef typename PropertyValueIterator<T>::difference_type difference_type;
        typedef typename PropertyValueIterator<T>::value_type      value_type;
        typedef typename PropertyValueIterator<T>::pointer         pointer;
        typedef typename PropertyValueIterator<T>::reference       reference;

        PropertyValueIterator(const PropertyValue &iteratee, std::size_t index);
        PropertyValueIterator(const PropertyValueIterator<T> &other);
        ~PropertyValueIterator();

        value_type operator*() const;

        bool operator!=(const PropertyValueIterator<T> &other) const;
        bool operator==(const PropertyValueIterator<T> &other) const;

        PropertyValueIterator<T>& operator++();
        PropertyValueIterator<T> operator++(int);

    private:
        const PropertyValue &m_propval;
        std::size_t m_index;
    };

    class PropertyValue {
    public:
        ////////////////////////////////////////////////////////////

        PropertyValue();
        PropertyValue(const PropertyValue &other);
        PropertyValue(PropertyValue &&other);
        ~PropertyValue();

        PropertyValue& operator=(const PropertyValue &other);
        PropertyValue& operator=(PropertyValue &&other);

        ////////////////////////////////////////////////////////////

        template<typename T>
        PropertyValue(T s_val);

        template<typename T>
        PropertyValue(const std::vector<T> &v_val);

        ////////////////////////////////////////////////////////////

        template<typename T>
        PropertyValue& operator=(T s_val);

        template<typename T>
        PropertyValue& operator=(const std::vector<T> &v_val);

        ////////////////////////////////////////////////////////////

        bool isList() const;
        bool isIntegral() const;
        std::size_t size() const;

        ////////////////////////////////////////////////////////////

        template<typename T>
        T first() const;

        template<typename T>
        PropertyValueIterator<T> begin() const;

        template<typename T>
        PropertyValueIterator<T> end() const;

        ////////////////////////////////////////////////////////////

        template<typename T>
        friend class PropertyValueIterator;
        friend class Element;

        friend std::ostream& operator<<(std::ostream&, const PropertyValue&);

    private:
        class Impl;
        std::unique_ptr<Impl> m_value;
    };

    std::ostream& operator<<(std::ostream& stream, const PropertyValue &pv);

    class Element {
    public:
        Element(const char *name, int count);
        Element(const Element &other);
        Element(Element &&other);
        ~Element();

        Element& operator=(const Element &other);
        Element& operator=(Element &&other);

        const std::string& name() const;
        const char* name_c() const;
        int count() const;
        const std::vector<Property>& properties() const;
        const std::vector<ElementValue>& data() const;

        friend class PlyFile;

    private:
        void addProperty(Property &&prop);
        void loadAsciiData(std::istream &stream);
        void loadBinaryData(std::istream &stream, Format format);

        std::string m_name;
        int m_count;
        std::vector<Property> m_props;
        std::vector<ElementValue> m_data;
    };

    class ElementValue {
    public:
        ElementValue();
        ElementValue(const ElementValue &other);
        ElementValue(ElementValue &&other);
        ~ElementValue();

        ElementValue& operator=(const ElementValue &other);
        ElementValue& operator=(ElementValue &&other);

        const PropertyValue& getProperty(const std::string &pname) const;
        const PropertyValue& getProperty(const char *pname) const;

        friend class Element;

    private:
        std::map<std::string, PropertyValue> m_propvals;
    };

    class PlyFile {
    public:
        typedef std::vector<std::string>::size_type comment_size_type;
        typedef std::vector<std::string>::iterator comment_iterator;
        typedef std::vector<std::string>::const_iterator const_comment_iterator;

        typedef std::map<std::string, Element>::size_type element_size_type;
        typedef std::map<std::string, Element>::iterator element_iterator;
        typedef std::map<std::string, Element>::const_iterator const_element_iterator;

        PlyFile(const char *filename);
        PlyFile(std::istream &stream);
        PlyFile(const PlyFile &other) = delete;
        PlyFile(PlyFile &&other) = delete;
        ~PlyFile();

        PlyFile& operator=(const PlyFile &other) = delete;
        PlyFile& operator=(PlyFile &&other) = delete;

        comment_size_type numComments() const { return m_comments.size(); }
        comment_iterator beginComments() { return m_comments.begin(); }
        comment_iterator endComments() { return m_comments.end(); }
        const_comment_iterator cbeginComments() const { return m_comments.cbegin(); }
        const_comment_iterator cendComments() const { return m_comments.cend(); }

        element_size_type numElements() const { return m_elements.size(); }
        const Element* getElement(const std::string &pname) const;
        const Element* getElement(const char *pname) const;
        element_iterator beginElements() { return m_elements.begin(); }
        element_iterator endElements() { return m_elements.end(); }
        const_element_iterator cbeginElements() const { return m_elements.cbegin(); }
        const_element_iterator cendElements() const { return m_elements.cend(); }

    private:
        void load(std::istream &stream);
        void addElement(Element &&elem);

        std::vector<std::string> m_comments;
        std::map<std::string, Element> m_elements;
        std::vector<Element*> m_element_seq;
    };
}

#include "PlyFile.tmpl.cpp"

#endif
