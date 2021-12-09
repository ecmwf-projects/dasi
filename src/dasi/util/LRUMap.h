#pragma once

#include "dasi/util/Exceptions.h"
#include "dasi/util/ContainerIostream.h"

#include <chrono>
#include <map>
#include <list>
#include <initializer_list>

namespace dasi {

//----------------------------------------------------------------------------------------------------------------------

template <typename Key,
          typename T,
          typename Compare = std::less<Key>>
class LRUMap {

private: // types

    using TimeT = std::chrono::time_point<std::chrono::steady_clock>;

    struct TimeRefT;
    struct StoredT;
    using ListT = std::list<TimeRefT>;
    using MapT = std::map<Key, StoredT, Compare>;

    struct StoredT {
        typename ListT::const_iterator it;
        T value;
    };

    struct TimeRefT {
        TimeT time;
        typename MapT::iterator it;
    };

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
        reference operator*() const { return listIt_->it->second.value; }
        pointer operator->() const { return &listIt_->it->second.value; }
    };

public: // types

    using key_type = Key;
    using mapped_type = T;
    using value_type = typename MapT::value_type;
    using iterator = Iterator<typename MapT::value_type, typename ListT::iterator>;
    using const_iterator = Iterator<const typename MapT::value_type, typename ListT::const_iterator>;
    using size_type = typename MapT::size_type;

public: // methods

    LRUMap(size_t maxSize);
    LRUMap(size_t maxSize, std::initializer_list<value_type>);

    T& operator[](const Key& key) { return values_[key]; }

    iterator begin() { return iterator(times_.begin()); }
    iterator end() { return iterator(times_.end()); }
    const_iterator begin() const { return const_iterator(times_.begin()); }
    const_iterator end() const { return const_iterator(times_.end()); }

    template <typename LookupKey>
    iterator find(const LookupKey& k) { return iterator(values_.find(k)->second.it); }
    template <typename LookupKey>
    const_iterator find(const LookupKey& k) const { return const_iterator(values_.find(k)->second.it); }

    [[ nodiscard ]]
    bool empty() const { return values_.empty(); }

    [[ nodiscard ]]
    size_type size() const { return values_.size(); }

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
    MapT values_;
    ListT times_;
};

//----------------------------------------------------------------------------------------------------------------------

template <typename Key, typename T, typename Compare>
LRUMap<Key, T, Compare>::LRUMap(size_t maxSize) : maxSize_(maxSize) {}

template <typename Key, typename T, typename Compare>
LRUMap<Key, T, Compare>::LRUMap(size_t maxSize, std::initializer_list<value_type> values) :
    maxSize_(maxSize) {
    if (values.size() > maxSize_) throw SeriousBug("Too many elements for LRUMap on construction", Here());
    times_.reserve(values.size());
    auto now = std::chrono::steady_clock::now();
    for (auto& kv : values) {
        auto r = values_.emplace(std::make_pair(std::move(kv.first), {times_.end(), std::move(kv.second)}));
        ASSERT(r.second);
        auto it2 = times_.insert(times_.end(), {now, r.first});
        r.first->it = it2;
    }
}

template <typename Key, typename T, typename Compare>
void LRUMap<Key, T, Compare>::print(std::ostream& s) const {
    s << "{";
    bool first = true;
    for (const auto& elem : times_) {
        if (!first) s << ", ";
        s << elem.it->first << ":" << elem.it->second;
        first = false;
    }
    s << "}";
}

template <typename Key, typename T, typename Compare>
auto LRUMap<Key, T, Compare>::emplace(std::pair<Key, T>&& val) -> std::pair<iterator, bool> {

    auto ins = values_.emplace(std::make_pair(std::move(val.first), {times_.end(), std::move(val.second)}));
    if (ins.second) {
        auto it_times = times_.emplace_front({std::chrono::steady_clock::now(), ins.first});
        ins.first->it = it_times;
    }

    // If we are full, remove the oldest entry.
    if (values_.size() > maxSize_) {
        auto ittime = times_.end();
        auto itval = ittime->it;
        values_.erase(itval);
        times_.erase(ittime);
    }

    return std::make_pair(iterator(ins.first), ins.second);
}

//----------------------------------------------------------------------------------------------------------------------

}