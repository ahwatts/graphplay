// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "PlyFile.h"

#include <fstream>
#include <iomanip>
#include <sstream>

#include <boost/endian/conversion.hpp>

namespace graphplay {
    typedef std::vector<std::string> StringVec;

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

    PropertyValue create_list(ListType type);

    Format read_format(const StringVec &toks);
    std::string read_comment(const StringVec &toks);
    ScalarType read_value_type(const std::string &type_str);
    Element read_element(const StringVec &toks);
    Property read_property(const StringVec &toks);
    PropertyValue read_ascii_value(const std::string& token, ScalarType type);
    PropertyValue read_binary_value(std::istream &stream, ScalarType type, Format format);

    PropertyValue read_float_binary_value(std::istream &stream, Format format);
    PropertyValue read_double_binary_value(std::istream &stream, Format format);
    template<typename T>
    PropertyValue read_int_binary_value(std::istream &stream, Format format);

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

    void PlyFile::debug(std::ostream &out) const {
        std::cout << std::boolalpha;

        std::cout << "Comments:" << std::endl;
        for (auto&& c : m_comments) {
            std::cout << "  " << c << std::endl;
        }

        std::cout << "Elements:" << std::endl;
        for (auto&& e : m_elements) {
            std::cout << "  " << e.name() << " (count: " << e.count() << ") Properties:" << std::endl;

            for (auto&& p : e.properties()) {
                std::cout << "    " << p.name() << " integral: " << p.isIntegral() << " list: " << p.isList() << std::endl;
            }
        }

        std::cout << std::resetiosflags(std::ios_base::boolalpha);
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
            } else {
                e.loadBinaryData(stream, format);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Implementation of class Element.
    ////////////////////////////////////////////////////////////////////////////////

    Element::Element(const char *name, int count)
        : m_name{ name },
        m_count{ count },
        m_props{},
        m_data{}
    {}

    Element::Element(const Element &other)
        : m_name{ other.m_name },
        m_count{ other.m_count },
        m_props{ other.m_props },
        m_data{ other.m_data }
    {}

    Element::Element(Element &&other)
        : m_name{},
        m_count{ other.m_count },
        m_props{},
        m_data{}
    {
        std::swap(m_name, other.m_name);
        std::swap(m_props, other.m_props);
        std::swap(m_data, other.m_data);
    }

    Element::~Element() {}

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
                    ListType type = boost::get<ListType>(prop->type());
                    PropertyValue count_val = read_ascii_value(*token, type.count_type);
                    int count = boost::apply_visitor(casting_visitor<int>(), count_val);
                    PropertyValue list_val = create_list(type);

                    for (int i = 0; i < count && token != tokens.end(); ++i) {
                        ++token;
                        PropertyValue val = read_ascii_value(*token, type.value_type);
                        boost::apply_visitor(append_visitor(), list_val, val);
                    }

                    elem.emplace(prop->name(), list_val);
                } else {
                    elem.emplace(prop->name(), read_ascii_value(*token, boost::get<ScalarType>(prop->type())));
                }
                ++token;
                ++prop;
            }

            m_data.emplace_back(std::move(elem));
        }
    }

    void Element::loadBinaryData(std::istream &stream, Format format) {
        for (int row = 0; row < m_count && !stream.eof(); ++row) {
            ElementValue elem;

            for (auto &&prop : m_props) {
                if (prop.isList()) {
                    ListType type = boost::get<ListType>(prop.type());
                    PropertyValue count_val = read_binary_value(stream, type.count_type, format);
                    int count = boost::apply_visitor(casting_visitor<int>(), count_val);
                    PropertyValue list_val = create_list(type);

                    for (int i = 0; i < count; ++i) {
                        PropertyValue val = read_binary_value(stream, type.value_type, format);
                        boost::apply_visitor(append_visitor(), list_val, val);
                    }

                    elem.emplace(prop.name(), list_val);
                } else {
                    elem.emplace(prop.name(), read_binary_value(stream, boost::get<ScalarType>(prop.type()), format));
                }
            }

            m_data.emplace_back(std::move(elem));
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Implementation of class Property
    ////////////////////////////////////////////////////////////////////////////////

    Property::Property(const char *name, int offset, ScalarType type)
        : m_name{ name },
        m_type{ type }
    {}

    Property::Property(const char *name, int offset, ListType type)
        : m_name{ name },
        m_type{ type }
    {}

    Property::Property(const Property &other)
        : m_name{ other.m_name },
        m_type{ other.m_type }
    {}

    Property::Property(Property &&other)
        : m_name{},
        m_type{ other.m_type }
    {
        std::swap(m_name, other.m_name);
    }

    Property::~Property() {}

    bool Property::isList() const {
        return boost::apply_visitor(is_list_visitor(), m_type);
    }

    bool Property::isIntegral() const {
        return boost::apply_visitor(is_integral_visitor(), m_type);
    }

    std::ostream& operator<<(std::ostream& stream, const PropertyValue &value) {
        bool is_list = boost::apply_visitor(is_list_visitor(), value);
        bool is_integer = boost::apply_visitor(is_integral_visitor(), value);

        if (is_list && is_integer) {
            const std::vector<std::int64_t> &idata = boost::get<std::vector<std::int64_t> >(value);
            std::ostringstream itemp;

            itemp << "[ ";
            for (unsigned int i = 0; i < idata.size(); ++i) {
                if (i == idata.size() - 1) {
                    itemp << idata[i];
                } else {
                    itemp << idata[i] << ", ";
                }
            }
            itemp << " ]";

            return stream << itemp.str();
        } else if (is_list && !is_integer) {
            const std::vector<double> &fdata = boost::get<std::vector<double> >(value);
            std::ostringstream dtemp;

            dtemp << "[ ";
            for (unsigned int j = 0; j < fdata.size(); ++j) {
                if (j == fdata.size() - 1) {
                    dtemp << fdata[j];
                } else {
                    dtemp << fdata[j] << ", ";
                }
            }
            dtemp << " ]";

            return stream << dtemp.str();
        } else if (!is_list && is_integer) {
            return stream << boost::get<std::int64_t>(value);
        } else { // !is_list && !is_integer
            return stream << boost::get<double>(value);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Implementation of internal helper functions for PlyFile.
    ////////////////////////////////////////////////////////////////////////////////

    std::ostream& operator<<(std::ostream &stream, Format value) {
        switch (value) {
        case ASCII:
            return stream << "ASCII";
        case BINARY_BIG_ENDIAN:
            return stream << "binary (big-endian)";
        case BINARY_LITTLE_ENDIAN:
            return stream << "binary (little-endian)";
        default:
            return stream;
        }
    }

    PropertyValue create_list(ListType type) {
        PropertyValue rv;

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
            return Property(toks[4].c_str(), 0, ListType{ read_value_type(toks[2]), read_value_type(toks[3]) });
        } else if (!list && toks.size() >= 3) {
            return Property(toks[2].c_str(), 0, read_value_type(toks[1]));
        } else {
            return Property("", 0, UINT_8);
        }
    }

    PropertyValue read_ascii_value(const std::string& token, ScalarType type) {
        PropertyValue value;

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
            std::cerr << "Cannot handle PLY value type (?) with value " << token << std::endl;
            break;
        }

        return value;
    }

    template<typename T>
    PropertyValue read_int_binary_value(std::istream &stream, Format format) {
        T temp;
        PropertyValue rv;

        stream.read(reinterpret_cast<char*>(&temp), sizeof(temp));

        if (format == BINARY_BIG_ENDIAN) {
            boost::endian::big_to_native_inplace(temp);
        } else if (format == BINARY_LITTLE_ENDIAN) {
            boost::endian::little_to_native_inplace(temp);
        }

        rv = static_cast<std::int64_t>(temp);
        return rv;
    }

    PropertyValue read_float_binary_value(std::istream &stream, Format format) {
        std::uint32_t itemp;
        PropertyValue rv;

        stream.read(reinterpret_cast<char*>(&itemp), sizeof(itemp));

        if (format == BINARY_BIG_ENDIAN) {
            boost::endian::big_to_native_inplace(itemp);
        } else if (format == BINARY_LITTLE_ENDIAN) {
            boost::endian::little_to_native_inplace(itemp);
        }

        rv = static_cast<double>(*reinterpret_cast<float*>(&itemp));
        return rv;
    }

    PropertyValue read_double_binary_value(std::istream &stream, Format format) {
        std::uint64_t itemp;
        PropertyValue rv;

        stream.read(reinterpret_cast<char*>(&itemp), sizeof(itemp));

        if (format == BINARY_BIG_ENDIAN) {
            boost::endian::big_to_native_inplace(itemp);
        } else if (format == BINARY_LITTLE_ENDIAN) {
            boost::endian::little_to_native_inplace(itemp);
        }

        rv = *reinterpret_cast<double*>(&itemp);
        return rv;
    }

    PropertyValue read_binary_value(std::istream &stream, ScalarType type, Format format) {
        switch (type) {
        case UINT_8:
            return read_int_binary_value<std::uint8_t>(stream, format);
        case INT_8:
            return read_int_binary_value<std::int8_t>(stream, format);
        case UINT_16:
            return read_int_binary_value<std::uint16_t>(stream, format);
        case INT_16:
            return read_int_binary_value<std::int16_t>(stream, format);
        case UINT_32:
            return read_int_binary_value<std::uint32_t>(stream, format);
        case INT_32:
            return read_int_binary_value<std::int32_t>(stream, format);
        case FLOAT_32:
            return read_float_binary_value(stream, format);
        case FLOAT_64:
            return read_double_binary_value(stream, format);
        default:
            return PropertyValue();
        }
    }

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