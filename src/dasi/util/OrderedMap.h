#pragma once

#include "dasi/util/Exceptions.h"
#include "dasi/util/ContainerIostream.h"

#include <map>
#include <vector>
#include <initializer_list>

namespace dasi {

//----------------------------------------------------------------------------------------------------------------------

template <typename Key,
          typename T,
          typename Compare = std::less<Key>,
          typename Allocator = std::allocator<std::pair<const Key, T>>,
          typename VecAllocator = std::allocator<typename std::map<Key, T, Compare, Allocator>::const_iterator>>
class OrderedMap {

private: // types

    using MapT = std::map<Key, T, Compare, Allocator>;
    using VecT = std::vector<typename MapT::iterator, VecAllocator>;

    template <typename ValT, typename MapItT, typename VecItT>
    class Iterator {
        bool iterable_ = false;
        MapItT mapIt_;
        VecItT vecIt_;

    public: // traits
        using difference_type = std::ptrdiff_t;
        using value_type = ValT;
        using reference = ValT&;
        using pointer = ValT*;
        using iterator_category = std::forward_iterator_tag;
    public:
        Iterator() = default;
        explicit Iterator(MapItT it) : iterable_(false), mapIt_(it) {}
        explicit Iterator(VecItT it) : iterable_(true), vecIt_(it) {}
        Iterator(MapItT mit, VecItT vit) : iterable_(false), mapIt_(mit), vecIt_(vit) {}
        bool operator==(const Iterator& rhs) { return iterable_ ? (vecIt_ == rhs.vecIt_) : (mapIt_ == rhs.mapIt_); }
        bool operator!=(const Iterator& rhs) { return iterable_ ? (vecIt_ != rhs.vecIt_) : (mapIt_ != rhs.mapIt_); }
        Iterator<ValT, MapItT, VecItT>& operator=(const Iterator<ValT, MapItT, VecItT>& rhs) {
            iterable_ = rhs.iterable_;
            mapIt_ = rhs.mapIt_;
            vecIt_ = rhs.vecIt_;
            return *this;
        }
        Iterator<ValT, MapItT, VecItT>& operator++() {
            if (!iterable_) NOTIMP;
            ++vecIt_;
            return *this;
        }
        reference operator*() const { return iterable_ ? **vecIt_ : *mapIt_; }
        pointer operator->() const { return iterable_ ? &**vecIt_ : &*mapIt_; }
    };

public: // types

    using key_type = Key;
    using mapped_type = T;
    using value_type = typename MapT::value_type;
    using iterator = Iterator<typename MapT::value_type, typename MapT::iterator, typename VecT::iterator>;
    using const_iterator = Iterator<const typename MapT::value_type, typename MapT::const_iterator, typename VecT::const_iterator>;
    using size_type = typename MapT::size_type;

public: // methods

    OrderedMap() = default;
    OrderedMap(std::initializer_list<value_type>);

    T& operator[](const Key& key) { return values_[key]; }

    iterator begin() { return iterator(keys_.begin()); }
    iterator end() { return iterator(values_.end(), keys_.end()); }
    const_iterator begin() const { return const_iterator(keys_.begin()); }
    const_iterator end() const { return const_iterator(values_.end(), keys_.end()); }

    template <typename LookupKey>
    iterator find(const LookupKey& k) { return iterator(values_.find(k)); }
    template <typename LookupKey>
    const_iterator find(const LookupKey& k) const { return const_iterator(values_.find(k)); }

    [[ nodiscard ]]
    bool empty() const { return values_.empty(); }

    [[ nodiscard ]]
    size_type size() const { return values_.size(); }

    std::pair<iterator, bool> insert(const std::pair<Key, T>& val);

    template <typename M>
    std::pair<iterator, bool> insert_or_assign(const Key&, M&& val);

private: // methods

    void print(std::ostream& s) const;

private: // friends

    friend std::ostream& operator<<(std::ostream& s, const OrderedMap<Key, T, Compare, Allocator, VecAllocator>& om) {
        om.print(s);
        return s;
    }

private: // members

    MapT values_;
    std::vector<typename MapT::iterator, VecAllocator> keys_;
};

//----------------------------------------------------------------------------------------------------------------------

template <typename Key, typename T, typename Compare, typename Allocator, typename VecAllocator>
OrderedMap<Key, T, Compare, Allocator, VecAllocator>::OrderedMap(std::initializer_list<value_type> values) {
    keys_.reserve(values.size());
    for (auto& kv : values) {
        auto r = values_.emplace(std::move(kv));
        ASSERT(r.second);
        keys_.emplace_back(std::move(r.first));
    }
}

template <typename Key, typename T, typename Compare, typename Allocator, typename VecAllocator>
void OrderedMap<Key, T, Compare, Allocator, VecAllocator>::print(std::ostream& s) const {
    s << "{";
    bool first = true;
    for (const auto& elem : keys_) {
        if (!first) s << ", ";
        s << elem->first << ":" << elem->second;
        first = false;
    }
    s << "}";
}

template <typename Key, typename T, typename Compare, typename Allocator, typename VecAllocator>
auto OrderedMap<Key, T, Compare, Allocator, VecAllocator>::insert(const std::pair<Key, T>& val) -> std::pair<iterator, bool> {
    auto mp_ins = values_.insert(val);
    if (mp_ins.second) {
        keys_.push_back(mp_ins.first);
    }
    return std::make_pair(iterator(mp_ins.first), mp_ins.second);
}

template <typename Key, typename T, typename Compare, typename Allocator, typename VecAllocator>
template <typename M>
auto OrderedMap<Key, T, Compare, Allocator, VecAllocator>::insert_or_assign(const Key& key, M&& val) -> std::pair<iterator, bool> {
    auto ins_ret = insert(std::make_pair(key, val));
    if (!ins_ret.second) ins_ret.first->second = val;
    return ins_ret;
}

//----------------------------------------------------------------------------------------------------------------------

}

namespace std {
template<typename S, typename T, typename C, typename A>
inline std::ostream& operator<<(std::ostream& s, const ::dasi::OrderedMap<S, T, C, A>& v) {
    ::dasi::internal::print_map(s, v);
    return s;
}
}