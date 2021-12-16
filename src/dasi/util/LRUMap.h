#pragma once

#include "dasi/util/Exceptions.h"
#include "dasi/util/ContainerIostream.h"

#include <chrono>
#include <map>
#include <list>
#include <initializer_list>

namespace dasi::util {

//----------------------------------------------------------------------------------------------------------------------

template <typename Key,
          typename T,
          typename Compare = std::less<Key>>
class LRUMap {

private: // types

    using TimeT = std::chrono::time_point<std::chrono::steady_clock>;

    struct DataT {
        TimeT time;
        std::pair<Key, T> value;
    };

    using ListT = std::list<DataT>;
    using MapT = std::map<Key, typename ListT::const_iterator>;

    template <typename ValT, typename ListItT>
    class Iterator {
        ListItT listIt_;

    public: // traits
        using difference_type = std::ptrdiff_t;
        using value_type = ValT;
        using reference = ValT&;
        using pointer = ValT*;
        using iterator_category = std::forward_iterator_tag;
    public:
        Iterator() = default;
        explicit Iterator(ListItT it) : listIt_(it) {}
        bool operator==(const Iterator& rhs) { return listIt_ == rhs.listIt_; }
        bool operator!=(const Iterator& rhs) { return listIt_ != rhs.listIt_; }
        Iterator<ValT, ListItT>& operator=(const Iterator<ValT, ListItT>& rhs) {
            listIt_ = rhs.listIt_;
            return *this;
        }
        Iterator<ValT, ListItT>& operator++() {
            ++listIt_;
            return *this;
        }
        reference operator*() const { return listIt_->value; }
        pointer operator->() const { return &listIt_->value; }
    };

public: // types

    using key_type = Key;
    using mapped_type = T;
    using value_type = decltype(DataT::value);
    using iterator = Iterator<value_type, typename ListT::iterator>;
    using const_iterator = Iterator<const value_type, typename ListT::const_iterator>;
    using size_type = typename MapT::size_type;

public: // methods

    LRUMap(size_t maxSize);
    LRUMap(size_t maxSize, std::initializer_list<value_type>);

    LRUMap(LRUMap&&) = delete;
    LRUMap(const LRUMap&) = delete;
    LRUMap& operator=(LRUMap&&) = delete;
    LRUMap& operator=(const LRUMap&) = delete;

    const T& operator[](const Key& key) const;

    iterator begin() { return iterator(values_.begin()); }
    iterator end() { return iterator(values_.end()); }
    const_iterator begin() const { return const_iterator(values_.begin()); }
    const_iterator end() const { return const_iterator(values_.end()); }

    template <typename LookupKey>
    iterator find(const LookupKey& k) { return iterator(lookup_.find(k)->second.it); }
    template <typename LookupKey>
    const_iterator find(const LookupKey& k) const { return const_iterator(lookup_.find(k)->second.it); }

    [[ nodiscard ]]
    bool empty() const { return lookup_.empty(); }

    [[ nodiscard ]]
    size_type size() const { return lookup_.size(); }

    [[ nodiscard ]]
    size_type capacity() const { return maxSize_; }

    std::pair<iterator, bool> insert(const std::pair<Key, T>& val);
    std::pair<iterator, bool> insert(std::pair<Key, T>&& val);
    std::pair<iterator, bool> emplace(std::pair<Key, T>&& val);

private: // methods

    void print(std::ostream& s) const;

private: // friends

    friend std::ostream& operator<<(std::ostream& s, const LRUMap<Key, T, Compare>& om) {
        om.print(s);
        return s;
    }

private: // members

    size_t maxSize_;
    MapT lookup_;
    ListT values_;
};

//----------------------------------------------------------------------------------------------------------------------

template <typename Key, typename T, typename Compare>
LRUMap<Key, T, Compare>::LRUMap(size_t maxSize) : maxSize_(maxSize) {}

template <typename Key, typename T, typename Compare>
LRUMap<Key, T, Compare>::LRUMap(size_t maxSize, std::initializer_list<value_type> values) :
    maxSize_(maxSize) {
    if (values.size() > maxSize_) throw SeriousBug("Too many elements for LRUMap on construction", Here());
    auto now = std::chrono::steady_clock::now();
    for (auto& kv : values) {
        auto it = values_.emplace_back({now, std::move(kv)});
        auto r = lookup_.emplace(std::make_pair(it->value.first, it));
        ASSERT(r.second);
    }
}

template <typename Key, typename T, typename Compare>
const T& LRUMap<Key, T, Compare>::operator[](const Key& key) const {
    auto it = lookup_.find(key);
    if (it == lookup_.end()) throw KeyError("Invalid LRU value", Here());
    return it->second->value.second;
}

template <typename Key, typename T, typename Compare>
void LRUMap<Key, T, Compare>::print(std::ostream& s) const {
    s << "{";
    bool first = true;
    for (const auto& elem : values_) {
        if (!first) s << ", ";
        s << elem.value.first << ":" << elem.value.second;
        first = false;
    }
    s << "}";
}

template <typename Key, typename T, typename Compare>
auto LRUMap<Key, T, Compare>::insert(const std::pair<Key, T>& val) -> std::pair<iterator, bool> {

    bool inserted = false;
    auto itlist = values_.end();

    auto it = lookup_.find(val.first);
    if (it == lookup_.end()) {
        auto now = std::chrono::steady_clock::now();
        itlist = values_.emplace(values_.begin(), DataT{now, val});
        lookup_.emplace(val.first, itlist);
        inserted = true;
    }

    // If we are full, remove the oldest entry.
    if (lookup_.size() > maxSize_) {
        auto itlookup = lookup_.find(values_.back().value.first);
        ASSERT(itlookup != lookup_.end());
        lookup_.erase(itlookup);
        values_.pop_back();
    }

    return std::make_pair(iterator(itlist), inserted);
}

template <typename Key, typename T, typename Compare>
auto LRUMap<Key, T, Compare>::insert(std::pair<Key, T>&& val) -> std::pair<iterator, bool> {
    emplace(std::move(val));
}

template <typename Key, typename T, typename Compare>
auto LRUMap<Key, T, Compare>::emplace(std::pair<Key, T>&& val) -> std::pair<iterator, bool> {

    bool inserted = false;
    auto itlist = values_.end();

    auto it = lookup_.find(val.first);
    if (it == lookup_.end()) {
        auto now = std::chrono::steady_clock::now();
        itlist = values_.emplace(values_.begin(), {now, std::move(val)});
        lookup_.emplace(val.first, itlist->value.first);
        inserted = true;
    }

    // If we are full, remove the oldest entry.
    if (lookup_.size() > maxSize_) {
        auto itlookup = lookup_.find(values_.back().value.first);
        ASSERT(itlookup != lookup_.end());
        lookup_.erase(itlookup);
        values_.pop_back();
    }

    return std::make_pair(iterator(itlist), inserted);
}

//----------------------------------------------------------------------------------------------------------------------

}