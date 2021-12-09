
#pragma once

#include "dasi/util/Exceptions.h"
#include "dasi/util/StringBuilder.h"

#include <string>
#include <iosfwd>
#include <initializer_list>
#include <utility>

namespace dasi {

//----------------------------------------------------------------------------------------------------------------------

template <template<typename, typename, typename> class MapType, typename V>
class KeyT {

public: // types

    using value_type = V;

    // n.b. use of transparent comparator --> allow lookup with std::string_view as key
    using map_type = MapType<std::string, value_type, std::less<>>;

public: // methods

    KeyT() = default;
    KeyT(std::initializer_list<std::pair<const std::string, value_type>>);

    [[nodiscard]]
    bool has(const char* name) const;
    [[nodiscard]]
    bool has(const std::string_view& name) const;
    [[nodiscard]]
    bool has(const std::string& name) const;

    typename map_type::iterator set(const std::string& k, const std::string_view& v);

    const value_type& get(const std::string& k) const;
    const value_type& get(const std::string_view& k) const;

    template <template<typename, typename, typename> class MapType2, typename V2>
    bool operator==(const KeyT<MapType2, V2>& rhs) const;

private: // methods

    void print(std::ostream& s) const;

private: // friends

    friend std::ostream& operator<<(std::ostream& s, const KeyT& k) {
        k.print(s);
        return s;
    }

    template <template<typename, typename, typename> class MapType2, typename V2>
    friend class KeyT;

private: // members

    map_type values_;
};

//----------------------------------------------------------------------------------------------------------------------

template <template<typename, typename, typename> class MapType, typename V>
KeyT<MapType, V>::KeyT(std::initializer_list<std::pair<const std::string, value_type>> l) :
    values_(l) {}

template <template<typename, typename, typename> class MapType, typename V>
void KeyT<MapType, V>::print(std::ostream& s) const {
    s << values_;
}

template <template<typename, typename, typename> class MapType, typename V>
bool KeyT<MapType, V>::has(const char* name) const {
    return has(std::string_view(name));
}

template <template<typename, typename, typename> class MapType, typename V>
bool KeyT<MapType, V>::has(const std::string_view& name) const {
    return (values_.find(name) != values_.end());
}

template <template<typename, typename, typename> class MapType, typename V>
bool KeyT<MapType, V>::has(const std::string& name) const {
    return (values_.find(name) != values_.end());
}

template <template<typename, typename, typename> class MapType, typename V>
auto KeyT<MapType, V>::set(const std::string& k, const std::string_view& v) -> typename map_type::iterator {
    return values_.insert_or_assign(k, v).first;
}

template <template<typename, typename, typename> class MapType, typename V>
auto KeyT<MapType, V>::get(const std::string& name) const -> const value_type& {
    auto it = values_.find(name);
    if (it == values_.end()) throw KeyError((StringBuilder() << name << " not found in KeyT").str(), Here());
    return it->second;
}

template <template<typename, typename, typename> class MapType, typename V>
auto KeyT<MapType, V>::get(const std::string_view& name) const -> const value_type& {
    auto it = values_.find(name);
    if (it == values_.end()) throw KeyError((StringBuilder() << name << " not found in KeyT").str(), Here());
    return it->second;
}

template <template<typename, typename, typename> class MapType, typename V>
template <template<typename, typename, typename> class MapType2, typename V2>
bool KeyT<MapType, V>::operator==(const KeyT<MapType2, V2>& rhs) const {
    if (values_.size() != rhs.values_.size()) return false;
    for (const auto& kv : values_) {
        auto it = rhs.values_.find(kv.first);
        if (it == rhs.values_.end()) return false;
        if (kv.second != it->second) return false;
    }
    return true;
}

//----------------------------------------------------------------------------------------------------------------------

}