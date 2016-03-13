// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_PLY_FILE_H_
#define _GRAPHPLAY_GRAPHPLAY_PLY_FILE_H_

#include <cstdint>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <type_traits>

#include <boost/variant.hpp>

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
    class Property;
    typedef boost::variant<ScalarType, ListType> PropertyType;
    typedef boost::variant<std::int64_t, double, std::vector<std::int64_t>, std::vector<double> > PropertyValue;
    typedef std::map<std::string, PropertyValue> ElementValue;

    class Property {
    public:
        Property(const char *name, int offset, ScalarType type);
        Property(const char *name, int offset, ListType type);
        Property(const Property &other);
        Property(Property &&other);
        ~Property();

        Property& operator=(const Property &other) = delete;
        Property& operator=(Property &&other) = delete;

        inline const std::string& name() const { return m_name; }
        inline const char* name_c() const { return m_name.c_str(); }
        bool isList() const;
        bool isIntegral() const;
        const PropertyType& type() const { return m_type; }

        friend class Element;

    private:
        std::string m_name;
        PropertyType m_type;
    };

    class Element {
    public:
        Element(const char *name, int count);
        Element(const Element &other);
        Element(Element &&other);
        ~Element();

        Element& operator=(const Element &other) = delete;
        Element& operator=(Element &&other) = delete;

        inline const std::string& name() const { return m_name; }
        inline const char* name_c() const { return m_name.c_str(); }
        inline int count() const { return m_count; }
        inline const std::vector<Property>& properties() const { return m_props; }
        inline const std::vector<ElementValue>& data() const { return m_data; }

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

    class PlyFile {
    public:
        PlyFile(const char *filename);
        PlyFile(std::istream &stream);
        PlyFile(const PlyFile &other) = delete;
        PlyFile(PlyFile &&other) = delete;
        ~PlyFile();

        PlyFile& operator=(const PlyFile &other) = delete;
        PlyFile& operator=(PlyFile &&other) = delete;

        void debug(std::ostream &out) const;

        inline const std::vector<std::string>& comments() const { return m_comments;  }
        inline const std::vector<Element>& elements() const { return m_elements; }

    private:
        void load(std::istream &stream);

        std::vector<std::string> m_comments;
        std::vector<Element> m_elements;
    };

    std::ostream& operator<<(std::ostream& stream, Format value);
    std::ostream& operator<<(std::ostream& stream, const PropertyValue &value);

    class is_list_visitor : public boost::static_visitor<bool> {
    public:
        // If we use this visitor on a type descriptor...
        bool operator()(const ScalarType &t) const { return false; }
        bool operator()(const ListType &t) const { return true; }

        // If we use this visitor on a value...
        bool operator()(const std::int64_t &v) const { return false; }
        bool operator()(const double &v) const { return false; }
        bool operator()(const std::vector<std::int64_t> &v) const { return true; }
        bool operator()(const std::vector<double> &v) const { return true; }
    };

    class is_integral_visitor : public boost::static_visitor<bool> {
    public:
        // If we use this visitor on a type descriptor...
        bool operator()(const ScalarType &t) const {
            switch (t) {
            case UINT_8:
            case UINT_16:
            case UINT_32:
            case INT_8:
            case INT_16:
            case INT_32:
                return true;
            case FLOAT_32:
            case FLOAT_64:
                return false;
            default:
                return false;
            }
        }

        bool operator()(const ListType &t) const {
            is_integral_visitor v;
            return v(t.value_type);
        }

        // If we use this visitor on a value...
        bool operator()(const std::int64_t &v) const { return true; }
        bool operator()(const double &v) const { return false; }
        bool operator()(const std::vector<std::int64_t> &v) const { return true; }
        bool operator()(const std::vector<double> &v) const { return false; }
    };

    template<typename T>
    class casting_visitor : public boost::static_visitor<T> {
    public:
        T operator()(const std::int64_t &v) const {
            if (std::is_arithmetic<T>::value) {
                return static_cast<T>(v);
            } else {
                throw std::string("Canot cast a scalar value to a non-arithmetic type.");
            }
        }

        T operator()(const double &v) const {
            if (!std::is_arithmetic<T>::value) {
                throw std::string("Canot cast a scalar value to a non-arithmetic type.");
            } else if (std::is_integral<T>::value) {
                return static_cast<T>(std::round(v));
            } else {
                return static_cast<T>(v);
            }
        }

        template<typename U>
        T operator()(const U &v) const {
            throw std::string("Cannot cast list variant value.");
        }
    };
}

#endif
