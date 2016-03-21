// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_PLY_FILE_CPP_
#define _GRAPHPLAY_GRAPHPLAY_PLY_FILE_CPP_

#include <sstream>

#include <boost/variant.hpp>

namespace graphplay {
    typedef boost::variant<std::size_t> IndexVariant;
    typedef boost::variant<std::int64_t, double, std::vector<std::int64_t>, std::vector<double> > PropertyValueVariant;

    template<typename T>
    class at_visitor : public boost::static_visitor<T> {
        static_assert(std::is_arithmetic<T>::value, "Can only cast PropertyValues to arithmetic types.");

    public:
        T operator()(const std::int64_t &v, const std::size_t &index) const {
            if (index == 0) {
                return static_cast<T>(v);
            } else {
                std::ostringstream temp;
                temp << "Index out of range: " << index << " != 0";
                throw std::out_of_range(temp.str());
            }
        }

        T operator()(const double &v, const std::size_t &index) const {
            if (index == 0) {
                if (std::is_integral<T>::value) {
                    return static_cast<T>(std::round(v));
                } else {
                    return static_cast<T>(v);
                }
            } else {
                std::ostringstream temp;
                temp << "Index out of range: " << index << " != 0";
                throw std::out_of_range(temp.str());
            }
        }

        T operator()(const std::vector<std::int64_t> &v, const std::size_t &index) const {
            return static_cast<T>(v.at(index));
        }

        T operator()(const std::vector<double> &v, const std::size_t &index) const {
            if (std::is_integral<T>::value) {
                return static_cast<T>(std::round(v[index]));
            } else {
                return static_cast<T>(v.at(index));
            }
        }
    };

    class PropertyValue::Impl {
    public:
        Impl();
        Impl(const Impl &other);
        Impl(Impl &&other);
        ~Impl();

        Impl(const PropertyValueVariant &val);
        Impl(PropertyValueVariant &&val);

        Impl& operator=(const Impl &other);
        Impl& operator=(Impl &&other);
        Impl& operator=(const PropertyValueVariant &val);
        Impl& operator=(PropertyValueVariant &&val);

        PropertyValueVariant inner;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // Implementation of class PropertyValueIterator
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    PropertyValueIterator<T>::PropertyValueIterator(const PropertyValue &pv, std::size_t index)
        : m_propval(pv),
          m_index(index)
    {}

    template<typename T>
    PropertyValueIterator<T>::PropertyValueIterator(const PropertyValueIterator &other)
        : m_propval(other.m_propval),
          m_index(other.m_index)
    {}

    template<typename T>
    PropertyValueIterator<T>::~PropertyValueIterator() {}

    template<typename T>
    typename PropertyValueIterator<T>::value_type PropertyValueIterator<T>::operator*() const {
        IndexVariant vindex(m_index);
        return boost::apply_visitor(at_visitor<T>(), m_propval.m_value->inner, vindex);
    }

    template<typename T>
    bool PropertyValueIterator<T>::operator!=(const PropertyValueIterator &other) const {
        return &m_propval != &other.m_propval || m_index != other.m_index;
    }

    template<typename T>
    bool PropertyValueIterator<T>::operator==(const PropertyValueIterator &other) const {
        return !(*this != other);
    }

    template<typename T>
    PropertyValueIterator<T>& PropertyValueIterator<T>::operator++() {
        m_index += 1;
        return *this;
    }

    template<typename T>
    PropertyValueIterator<T> PropertyValueIterator<T>::operator++(int) {
        PropertyValueIterator<T> rv(*this);
        m_index += 1;
        return rv;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Template implementations of class PropertyValue.
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    PropertyValue::PropertyValue(T s_val) : PropertyValue() {
        static_assert(std::is_arithmetic<T>::value, "Can only construct PropertyValues with arithmetic types.");
        *this = s_val;
    }

    template<typename T>
    PropertyValue::PropertyValue(const std::vector<T> &val) : PropertyValue() {
        static_assert(std::is_arithmetic<T>::value, "Can only construct PropertyValues with arithmetic types.");
        *this = val;
    }

    template<typename T>
    PropertyValue& PropertyValue::operator=(T s_val) {
        static_assert(std::is_arithmetic<T>::value, "Can only assign arithmetic types to PropertyValues.");

        if (std::is_integral<T>::value) {
            m_value->inner = static_cast<std::int64_t>(s_val);
        } else {
            m_value->inner = static_cast<double>(s_val);
        }

        return *this;
    }

    template<typename T>
    PropertyValue& PropertyValue::operator=(const std::vector<T> &val) {
        static_assert(std::is_arithmetic<T>::value, "Can only assign vectors of arithmetic types to PropertyValues.");

        if (std::is_integral<T>::value) {
            std::vector<std::int64_t> icasted;
            for (auto &&v : val) {
                icasted.emplace_back(static_cast<std::int64_t>(v));
            }
            m_value->inner = std::move(icasted);
        } else {
            std::vector<double> dcasted;
            for (auto &&w : val) {
                dcasted.emplace_back(static_cast<double>(w));
            }
            m_value->inner = std::move(dcasted);
        }

        return *this;
    }

    template<typename T>
    T PropertyValue::first() const {
        IndexVariant zero = (std::size_t)0;
        return boost::apply_visitor(at_visitor<T>(), m_value->inner, zero);
    }

    template<typename T>
    PropertyValueIterator<T> PropertyValue::begin() const {
        return PropertyValueIterator<T>(*this, 0);
    }

    template<typename T>
    PropertyValueIterator<T> PropertyValue::end() const {
        return PropertyValueIterator<T>(*this, size());
    }
}

#endif
