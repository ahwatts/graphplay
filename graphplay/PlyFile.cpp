// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "PlyFile.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <boost/endian/conversion.hpp>
#include <boost/variant.hpp>

namespace graphplay {
    typedef std::vector<std::string> StringVec;
    typedef boost::variant<ScalarType, ListType> InnerPropertyType;
    typedef boost::variant<std::int64_t, double, std::vector<std::int64_t>, std::vector<double> > InnerPropertyValue;

    class Property::_Type {
    public:
        _Type(ScalarType type) : inner(type) {}
        _Type(ListType type) : inner(type) {}
        InnerPropertyType inner;
    };

    class PropertyValue::_Value {
    public:
        _Value(InnerPropertyValue v) : inner(v) {}
        _Value(const _Value &other) : inner(other.inner) {}
        _Value(_Value &&other) : inner() { std::swap(inner, other.inner); }
        InnerPropertyValue inner;
    };

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

    class append_visitor : public boost::static_visitor<> {
    public:
        template<typename T>
        void operator()(std::vector<T> &vec, const T &val) const {
            vec.emplace_back(val);
        }

        template<typename T>
        void operator()(std::vector<T> &vec, const std::vector<T> &val) const {
            vec.insert(vec.end(), val.begin(), val.end());
        }

        template<typename T1, typename T2>
        void operator()(T1 &vec, const T2 &val) const {
            throw std::string("Cannot append mixed types.");
        }
    };

    InnerPropertyValue create_list(ListType type);
    PropertyValue create_pv_from_inner(InnerPropertyValue &&inner);

    Format read_format(const StringVec &toks);
    std::string read_comment(const StringVec &toks);
    ScalarType read_value_type(const std::string &type_str);
    Element read_element(const StringVec &toks);
    Property read_property(const StringVec &toks);
    InnerPropertyValue read_ascii_value(const std::string& token, ScalarType type);
    // PropertyValue read_binary_value(std::istream &stream, ScalarType type, Format format);

    // PropertyValue read_float_binary_value(std::istream &stream, Format format);
    // PropertyValue read_double_binary_value(std::istream &stream, Format format);
    // template<typename T>
    // PropertyValue read_int_binary_value(std::istream &stream, Format format);

    ////////////////////////////////////////////////////////////////////////////////
    // Generic utilities.
    ////////////////////////////////////////////////////////////////////////////////

    void chomp(std::string &str);

    StringVec  split(const std::string &str, char sep);
    StringVec& split(const std::string &str, char sep, StringVec &tokens);

    std::string  join(StringVec::const_iterator begin, StringVec::const_iterator end, char sep);
    std::string &join(StringVec::const_iterator begin, StringVec::const_iterator end, char sep, std::string &dest);

    ////////////////////////////////////////////////////////////////////////////////
    // Implementation of class PlyFile.
    ////////////////////////////////////////////////////////////////////////////////

    PlyFile::PlyFile(const char *filename)
        : m_comments{},
          m_elements{}
    {
        std::fstream stream(filename, std::ios::in | std::ios::binary);
        load(stream);
        stream.close();
    }

    PlyFile::PlyFile(std::istream &stream)
        : m_comments{},
          m_elements{}
    {
        load(stream);
    }

    PlyFile::~PlyFile() {}

    const std::vector<std::string>& PlyFile::comments() const {
        return m_comments;
    }

    const std::vector<Element>& PlyFile::elements() const {
        return m_elements;
    }

    void PlyFile::load(std::istream &stream) {
        Format format = ASCII;
        std::string magic, line;

        std::getline(stream, magic);
        chomp(magic);
        if (!(magic == "ply\r" || magic == "ply")) {
            std::cout << "magic != ply: " << magic << std::endl;
            return;
        }

        while (std::getline(stream, line)) {
            chomp(line);
            StringVec tokens = split(line, ' ');

            if (tokens.size() > 0) {
                if (tokens[0] == "format") {
                    format = read_format(tokens);
                } else if (tokens[0] == "comment") {
                    m_comments.emplace_back(read_comment(tokens));
                } else if (tokens[0] == "element") {
                    m_elements.emplace_back(read_element(tokens));
                } else if (tokens[0] == "property") {
                    if (!m_elements.empty()) {
                        Element &elem = m_elements.back();
                        elem.addProperty(read_property(tokens));
                    }
                } else if (tokens[0] == "end_header") {
                    break;
                }
            }
        }

        for (auto&& e : m_elements) {
            if (format == ASCII) {
                e.loadAsciiData(stream);
            // } else {
            //     e.loadBinaryData(stream, format);
            }
        }
    }

    // ////////////////////////////////////////////////////////////////////////////////
    // // Implementation of class Element.
    // ////////////////////////////////////////////////////////////////////////////////

    Element::Element(const char *name, int count)
        : m_name{name},
          m_count{count},
          m_props{},
          m_data{}
    {}

    Element::Element(const Element &other)
        : m_name{other.m_name},
          m_count{other.m_count},
          m_props{other.m_props},
          m_data{other.m_data}
    {}

    Element::Element(Element &&other)
        : m_name{},
          m_count{other.m_count},
          m_props{},
          m_data{}
    {
        std::swap(m_name, other.m_name);
        std::swap(m_props, other.m_props);
        std::swap(m_data, other.m_data);
    }

    Element::~Element() {}

    const std::string& Element::name() const {
        return m_name;
    }

    const char* Element::name_c() const {
        return m_name.c_str();
    }

    int Element::count() const {
        return m_count;
    }

    const std::vector<Property>& Element::properties() const {
        return m_props;
    }

    const std::vector<ElementValue>& Element::data() const {
        return m_data;
    }

    void Element::addProperty(Property &&prop) {
        m_props.emplace_back(prop);
    }

    void Element::loadAsciiData(std::istream &stream) {
        std::string line;
        std::ostringstream row_stream;
        std::ostringstream list_stream;

        for (int row = 0; row < m_count && std::getline(stream, line); ++row) {
            chomp(line);
            StringVec tokens = split(line, ' ');
            ElementValue elem;
            auto token = tokens.begin();
            auto prop = m_props.begin();

            while (token != tokens.end() && prop != m_props.end()) {
                if (prop->isList()) {
                    ListType type = boost::get<ListType>(prop->m_type->inner);
                    InnerPropertyValue count_val = read_ascii_value(*token, type.count_type);
                    int count = boost::apply_visitor(casting_visitor<int>(), count_val);
                    InnerPropertyValue list_val = create_list(type);

                    for (int i = 0; i < count && token != tokens.end(); ++i) {
                        ++token;
                        InnerPropertyValue val = read_ascii_value(*token, type.value_type);
                        boost::apply_visitor(append_visitor(), list_val, val);
                    }

                    elem.m_propvals.emplace(prop->name(), create_pv_from_inner(std::move(list_val)));
                } else {
                    ScalarType type = boost::get<ScalarType>(prop->m_type->inner);
                    InnerPropertyValue val = read_ascii_value(*token, type);
                    elem.m_propvals.emplace(prop->name(), create_pv_from_inner(std::move(val)));
                }
                ++token;
                ++prop;
            }

            m_data.emplace_back(std::move(elem));
        }
    }

    // void Element::loadBinaryData(std::istream &stream, Format format) {
    //     for (int row = 0; row < m_count && !stream.eof(); ++row) {
    //         ElementValue elem;

    //         for (auto &&prop : m_props) {
    //             if (prop.isList()) {
    //                 ListType type = boost::get<ListType>(prop.type());
    //                 PropertyValue count_val = read_binary_value(stream, type.count_type, format);
    //                 int count = boost::apply_visitor(casting_visitor<int>(), count_val);
    //                 PropertyValue list_val = create_list(type);

    //                 for (int i = 0; i < count; ++i) {
    //                     PropertyValue val = read_binary_value(stream, type.value_type, format);
    //                     boost::apply_visitor(append_visitor(), list_val, val);
    //                 }

    //                 elem.emplace(prop.name(), list_val);
    //             } else {
    //                 elem.emplace(prop.name(), read_binary_value(stream, boost::get<ScalarType>(prop.type()), format));
    //             }
    //         }

    //         m_data.emplace_back(std::move(elem));
    //     }
    // }

    ////////////////////////////////////////////////////////////////////////////////
    // Implementation of class ElementValue
    ////////////////////////////////////////////////////////////////////////////////

    ElementValue::ElementValue()
        : m_propvals{}
    {}

    ElementValue::ElementValue(const ElementValue &other)
        : m_propvals{other.m_propvals}
    {}

    ElementValue::ElementValue(ElementValue &&other)
        : m_propvals{}
    {
        std::swap(m_propvals, other.m_propvals);
    }

    ElementValue::~ElementValue() {}

    const PropertyValue& ElementValue::getProperty(const std::string &pname) const {
        auto found = m_propvals.find(pname);

        if (found == m_propvals.end()) {
            throw std::string("Could not find property.");
        }

        return found->second;
    }

    const PropertyValue& ElementValue::getProperty(const char *pname) const {
        auto found = m_propvals.find(pname);

        if (found == m_propvals.end()) {
            throw std::string("Could not find property.");
        }

        return found->second;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Implementation of class Property
    ////////////////////////////////////////////////////////////////////////////////

    Property::Property(const char *name, ScalarType type)
        : m_name{name},
          m_type{new Property::_Type(type)}
    {}

    Property::Property(const char *name, ListType type)
        : m_name{name},
          m_type{new Property::_Type(type)}
    {}

    Property::Property(const Property &other)
        : m_name{other.m_name},
          m_type{new Property::_Type(*other.m_type)}
    {}

    Property::Property(Property &&other)
        : m_name{},
          m_type{std::move(other.m_type)}
    {
        std::swap(m_name, other.m_name);
    }

    Property::~Property() {}

    const std::string& Property::name() const {
        return m_name;
    }

    const char* Property::name_c() const {
        return m_name.c_str();
    }

    bool Property::isList() const {
        return boost::apply_visitor(is_list_visitor(), m_type->inner);
    }

    bool Property::isIntegral() const {
        return boost::apply_visitor(is_integral_visitor(), m_type->inner);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Implementation of class PropertyValue
    ////////////////////////////////////////////////////////////////////////////////

    PropertyValue::PropertyValue(_Value &&v)
        : m_value{new PropertyValue::_Value(std::move(v))}
    {}

    PropertyValue::PropertyValue(const PropertyValue &other)
        : m_value{new PropertyValue::_Value(*other.m_value)}
    {}

    PropertyValue::PropertyValue(PropertyValue &&other)
        : m_value{std::move(other.m_value)}
    {}

    PropertyValue::~PropertyValue() {}

    PropertyValue& PropertyValue::operator=(PropertyValue other) {
        std::swap(m_value, other.m_value);
        return *this;
    }

    bool PropertyValue::isList() const {
        return boost::apply_visitor(is_list_visitor(), m_value->inner);
    }

    bool PropertyValue::isIntegral() const {
        return boost::apply_visitor(is_integral_visitor(), m_value->inner);
    }

    std::int64_t PropertyValue::intValue() const {
        return boost::get<std::int64_t>(m_value->inner);
    }

    double PropertyValue::doubleValue() const {
        return boost::get<double>(m_value->inner);
    }

    const std::vector<std::int64_t>& PropertyValue::intListValue() {
        return boost::get<std::vector<std::int64_t> >(m_value->inner);
    }

    const std::vector<double>& PropertyValue::doubleListValue() {
        return boost::get<std::vector<double> >(m_value->inner);
    }

    // ////////////////////////////////////////////////////////////////////////////////
    // // Implementation of internal helper functions for PlyFile.
    // ////////////////////////////////////////////////////////////////////////////////

    InnerPropertyValue create_list(ListType type) {
        InnerPropertyValue rv;

        switch (type.value_type) {
        case UINT_8:
        case UINT_16:
        case UINT_32:
        case INT_8:
        case INT_16:
        case INT_32:
            rv = std::vector<std::int64_t>();
            break;
        case FLOAT_32:
        case FLOAT_64:
            rv = std::vector<double>();
        }

        return rv;
    }

    PropertyValue create_pv_from_inner(InnerPropertyValue &&inner) {
        PropertyValue::_Value v(std::move(inner));
        return PropertyValue(std::move(v));
    }

    Format read_format(const StringVec &toks) {
        Format rv = ASCII;

        if (toks.size() >= 2) {
            if (toks[1] == "ascii") {
                rv = ASCII;
            } else if (toks[1] == "binary_big_endian") {
                rv = BINARY_BIG_ENDIAN;
            } else if (toks[1] == "binary_little_endian") {
                rv = BINARY_LITTLE_ENDIAN;
            }
        }

        return rv;
    }

    std::string read_comment(const StringVec &toks) {
        if (toks.size() >= 2) {
            return join(std::next(toks.cbegin()), toks.cend(), ' ');
        } else {
            return "";
        }
    }

    ScalarType read_value_type(const std::string &type_str) {
        if (type_str == "uint8" || type_str == "uchar") {
            return UINT_8;
        } else if (type_str == "int8" || type_str == "char") {
            return INT_8;
        } else if (type_str == "uint16" || type_str == "ushort") {
            return UINT_16;
        } else if (type_str == "int16" || type_str == "short") {
            return INT_16;
        } else if (type_str == "uint32" || type_str == "uint") {
            return UINT_32;
        } else if (type_str == "int32" || type_str == "int") {
            return INT_32;
        } else if (type_str == "float" || type_str == "float32") {
            return FLOAT_32;
        } else if (type_str == "double" || type_str == "float64") {
            return FLOAT_64;
        } else {
            // ???
            return UINT_8;
        }
    }

    Element read_element(const StringVec &toks) {
        if (toks.size() >= 3) {
            return Element(toks[1].c_str(), std::stoi(toks[2]));
        } else {
            return Element("", 0);
        }
    }

    Property read_property(const StringVec &toks) {
        std::string name;
        bool list = false;

        if (toks.size() >= 2) {
            if (toks[1] == "list") {
                list = true;
            } else {
                list = false;
            }
        }

        if (list && toks.size() >= 5) {
            return Property(toks[4].c_str(), ListType{ read_value_type(toks[2]), read_value_type(toks[3]) });
        } else if (!list && toks.size() >= 3) {
            return Property(toks[2].c_str(), read_value_type(toks[1]));
        } else {
            return Property("", UINT_8);
        }
    }

    InnerPropertyValue read_ascii_value(const std::string& token, ScalarType type) {
        InnerPropertyValue value;

        switch (type) {
        case UINT_8:
        case UINT_16:
        case UINT_32:
            value = (std::int64_t)std::stoul(token);
            break;
        case INT_8:
        case INT_16:
        case INT_32:
            value = (std::int64_t)std::stol(token);
            break;
        case FLOAT_32:
        case FLOAT_64:
            value = std::stod(token);
            break;
        default:
            throw std::string("Cannot handle PLY value type");
        }

        return value;
    }

    // template<typename T>
    // PropertyValue read_int_binary_value(std::istream &stream, Format format) {
    //     T temp;
    //     PropertyValue rv;

    //     stream.read(reinterpret_cast<char*>(&temp), sizeof(temp));

    //     if (format == BINARY_BIG_ENDIAN) {
    //         boost::endian::big_to_native_inplace(temp);
    //     } else if (format == BINARY_LITTLE_ENDIAN) {
    //         boost::endian::little_to_native_inplace(temp);
    //     }

    //     rv = static_cast<std::int64_t>(temp);
    //     return rv;
    // }

    // PropertyValue read_float_binary_value(std::istream &stream, Format format) {
    //     std::uint32_t itemp;
    //     PropertyValue rv;

    //     stream.read(reinterpret_cast<char*>(&itemp), sizeof(itemp));

    //     if (format == BINARY_BIG_ENDIAN) {
    //         boost::endian::big_to_native_inplace(itemp);
    //     } else if (format == BINARY_LITTLE_ENDIAN) {
    //         boost::endian::little_to_native_inplace(itemp);
    //     }

    //     rv = static_cast<double>(*reinterpret_cast<float*>(&itemp));
    //     return rv;
    // }

    // PropertyValue read_double_binary_value(std::istream &stream, Format format) {
    //     std::uint64_t itemp;
    //     PropertyValue rv;

    //     stream.read(reinterpret_cast<char*>(&itemp), sizeof(itemp));

    //     if (format == BINARY_BIG_ENDIAN) {
    //         boost::endian::big_to_native_inplace(itemp);
    //     } else if (format == BINARY_LITTLE_ENDIAN) {
    //         boost::endian::little_to_native_inplace(itemp);
    //     }

    //     rv = *reinterpret_cast<double*>(&itemp);
    //     return rv;
    // }

    // PropertyValue read_binary_value(std::istream &stream, ScalarType type, Format format) {
    //     switch (type) {
    //     case UINT_8:
    //         return read_int_binary_value<std::uint8_t>(stream, format);
    //     case INT_8:
    //         return read_int_binary_value<std::int8_t>(stream, format);
    //     case UINT_16:
    //         return read_int_binary_value<std::uint16_t>(stream, format);
    //     case INT_16:
    //         return read_int_binary_value<std::int16_t>(stream, format);
    //     case UINT_32:
    //         return read_int_binary_value<std::uint32_t>(stream, format);
    //     case INT_32:
    //         return read_int_binary_value<std::int32_t>(stream, format);
    //     case FLOAT_32:
    //         return read_float_binary_value(stream, format);
    //     case FLOAT_64:
    //         return read_double_binary_value(stream, format);
    //     default:
    //         return PropertyValue();
    //     }
    // }

    ////////////////////////////////////////////////////////////////////////////////
    // Implementation of utilities.
    ////////////////////////////////////////////////////////////////////////////////

    void chomp(std::string &str) {
        if (str.size() > 0) {
            auto last = str.end();
            --last;
            if (*last == '\r') {
                str.erase(last, str.end());
            }
        }
    }

    StringVec split(const std::string &str, char sep) {
        StringVec tokens;
        split(str, sep, tokens);
        return tokens;
    }

    StringVec& split(const std::string &str, char sep, StringVec &tokens) {
        std::istringstream stream(str);
        std::string token;

        while (std::getline(stream, token, sep)) {
            tokens.emplace_back(token);
        }

        return tokens;
    }

    std::string &join(StringVec::const_iterator begin, StringVec::const_iterator end, char sep, std::string &dest) {
        dest.append(join(begin, end, sep));
        return dest;
    }

    std::string join(StringVec::const_iterator begin, StringVec::const_iterator end, char sep) {
        std::ostringstream stream;

        for (auto iter = begin; iter != end; ++iter) {
            stream << *iter;
            if (std::next(iter) != end) {
                stream << sep;
            }
        }

        return stream.str();
    }
}
