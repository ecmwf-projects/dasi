#pragma once

#include "dasi/util/Exceptions.h"
#include "dasi/util/ContainerIostream.h"

#include <map>
#include <vector>
#include <initializer_list>

namespace dasi::util {

//----------------------------------------------------------------------------------------------------------------------

// TODO: Can we do better by using Compare = std::less<>
template <typename Key,
          typename T,
          typename Compare = std::less<Key>,
          typename Allocator = std::allocator<std::pair<const Key, T>>,
          typename VecAllocator = std::allocator<typename std::map<Key, T, Compare, Allocator>::iterator>>
class OrderedMap {

private: // types

    using MapT = std::map<Key, T, Compare, Allocator>;
    using VecT = std::vector<typename MapT::iterator, VecAllocator>;

    template <typename ValT, typename MapItT, typename VecItT>
    class Iterator {
        friend class OrderedMap;
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
    OrderedMap(const OrderedMap& om);
    OrderedMap(std::initializer_list<value_type>);

//    const T& operator[](const Key& key) const;
    T& operator[](const Key& key);

    [[ nodiscard ]] bool operator==(const OrderedMap& rhs) const;
    [[ nodiscard ]] bool operator!=(const OrderedMap& rhs) const;
    [[ nodiscard ]] bool operator<(const OrderedMap& rhs) const;

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

    void clear();

    template <typename LookupKey>
    void erase(const LookupKey& k);
    void erase(iterator it);

    [[ nodiscard ]]
    size_type size() const { return values_.size(); }

    std::pair<iterator, bool> insert(const std::pair<Key, T>& val);
    std::pair<iterator, bool> insert(std::pair<Key, T>&& val);
    std::pair<iterator, bool> emplace(std::pair<Key, T>&& val);

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
OrderedMap<Key, T, Compare, Allocator, VecAllocator>::OrderedMap(const OrderedMap& om) {
    keys_.reserve(om.size());
    for (const auto& kv : om) {
        auto r = values_.emplace(kv);
        ASSERT(r.second);
        keys_.emplace_back(std::move(r.first));
    }
}

template <typename Key, typename T, typename Compare, typename Allocator, typename VecAllocator>
OrderedMap<Key, T, Compare, Allocator, VecAllocator>::OrderedMap(std::initializer_list<value_type> values) {
    keys_.reserve(values.size());
    for (auto& kv : values) {
        auto r = values_.emplace(std::move(kv));
        ASSERT(r.second);
        keys_.emplace_back(std::move(r.first));
    }
}

//template <typename Key, typename T, typename Compare, typename Allocator, typename VecAllocator>
//const T& OrderedMap<Key, T, Compare, Allocator, VecAllocator>::operator[](const Key& key) const {
//    auto it = values_.find(key);
//    if (it == values_.end()) throw KeyError("Invalid OrderedMap key", Here());
//    return it->second;
//}

template <typename Key, typename T, typename Compare, typename Allocator, typename VecAllocator>
T& OrderedMap<Key, T, Compare, Allocator, VecAllocator>::operator[](const Key& key) {
    auto it = values_.find(key);
    if (it == values_.end()) {
        insert(std::make_pair(key, T{}));
    }
    return it->second;
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
auto OrderedMap<Key, T, Compare, Allocator, VecAllocator>::emplace(std::pair<Key, T>&& val) -> std::pair<iterator, bool> {
    auto mp_ins = values_.insert(val);
    if (mp_ins.second) {
        keys_.push_back(mp_ins.first);
    }
    return std::make_pair(iterator(mp_ins.first), mp_ins.second);
}

template <typename Key, typename T, typename Compare, typename Allocator, typename VecAllocator>
auto OrderedMap<Key, T, Compare, Allocator, VecAllocator>::insert(std::pair<Key, T>&& val) -> std::pair<iterator, bool> {
    return emplace(std::move(val));
}

template <typename Key, typename T, typename Compare, typename Allocator, typename VecAllocator>
void OrderedMap<Key, T, Compare, Allocator, VecAllocator>::clear() {
    values_.clear();
    keys_.clear();
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
bool OrderedMap<Key, T, Compare, Allocator, VecAllocator>::operator==(const OrderedMap& rhs) const {
    return values_ == rhs.values_ && keys_ == rhs.keys_;
}

template <typename Key, typename T, typename Compare, typename Allocator, typename VecAllocator>
bool OrderedMap<Key, T, Compare, Allocator, VecAllocator>::operator!=(const OrderedMap& rhs) const {
    return !this->operator==(rhs);
}

template <typename Key, typename T, typename Compare, typename Allocator, typename VecAllocator>
bool OrderedMap<Key, T, Compare, Allocator, VecAllocator>::operator<(const OrderedMap& rhs) const {

    if (values_ < rhs.values_) return true;
    if (values_ > rhs.values_) return false;

    // Compare the order of the keys, only if otherwise equal
    auto compare_fn = [](const typename MapT::iterator& lhs, const typename MapT::iterator& rhs) -> bool {
        return lhs->first < rhs->first;
    };
    return std::lexicographical_compare(keys_.begin(), keys_.end(),
                                        rhs.keys_.begin(), rhs.keys_.end(),
                                        compare_fn);
}

template <typename Key, typename T, typename Compare, typename Allocator, typename VecAllocator>
template <typename M>
auto OrderedMap<Key, T, Compare, Allocator, VecAllocator>::insert_or_assign(const Key& key, M&& val) -> std::pair<iterator, bool> {
    auto ins_ret = insert(std::make_pair(key, T{std::forward<M>(val)}));
    if (!ins_ret.second) ins_ret.first->second = val;
    return ins_ret;
}

// TODO: erase is horribly inefficient, due to the linear search.
// TODO: Rework OrderedMap to use same layout schema as LRUMap

template <typename Key, typename T, typename Compare, typename Allocator, typename VecAllocator>
template <typename LookupKey>
void OrderedMap<Key, T, Compare, Allocator, VecAllocator>::erase(const LookupKey& key) {
    auto it = values_.find(key);
    if (it == values_.end()) return;

    for (auto it_keys = keys_.begin(); it_keys != keys_.end(); ++it_keys) {
        if (*it_keys == it) {
            values_.erase(it);
            keys_.erase(it_keys);
            return;
        }
    }

    throw SeriousBug("Failed to erase key from ordered list", Here());
}

template <typename Key, typename T, typename Compare, typename Allocator, typename VecAllocator>
void OrderedMap<Key, T, Compare, Allocator, VecAllocator>::erase(iterator it) {

    if (it.iterable_) {
        auto vec_it = it.vecIt_;
        if (vec_it == keys_.end()) return;
        auto map_it = *vec_it;
        values_.erase(map_it);
        keys_.erase(vec_it);
    } else {
        auto map_it = it.mapIt_;
        if (map_it == values_.end()) return;
        for (auto vec_it = keys_.begin(); vec_it != keys_.end(); ++vec_it) {
            if (*vec_it == map_it) {
                values_.erase(map_it);
                keys_.erase(vec_it);
                return;
            }
        }
        throw SeriousBug("Failed to erase key from ordered list", Here());
    }
}

//----------------------------------------------------------------------------------------------------------------------

}