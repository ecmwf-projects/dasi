
#pragma once

#include "dasi/api/Key.h"

#include <iterator>
#include <vector>

namespace dasi::api {

class Handle;

//----------------------------------------------------------------------------------------------------------------------

class Result {

public: // types

    using item_type = Handle*;
    class const_iterator;

public: // methods

    Result(std::vector<Key>&& keys, std::vector<item_type>&& values);
    ~Result();

    const_iterator begin() const { return const_iterator{keys_.begin(), values_.begin()}; }
    const_iterator end() const { return const_iterator{keys_.end(), values_.end()}; }

    const std::vector<Key>& keys() const { return keys_; }
    Handle* toHandle();

private: // members

    std::vector<Key> keys_;
    std::vector<item_type> values_;

public: // classes

    class const_iterator : public std::iterator<
        std::input_iterator_tag,
        std::pair<Key, Result::item_type>> {

    private: // types

        using key_iterator = std::vector<Key>::const_iterator;
        using value_iterator = std::vector<Result::item_type>::const_iterator;

    public: // methods

        explicit const_iterator(key_iterator ki, value_iterator vi) :
            current_key_(ki), current_val_(vi) {}

        const_iterator& operator++() { ++current_key_; ++current_val_; return *this; }
        const_iterator operator++(int) { const_iterator retval = *this; ++(*this); return retval; }
        bool operator==(const_iterator other) const { return current_key_ == other.current_key_ && current_val_ == other.current_val_; }
        bool operator!=(const_iterator other) const { return !(*this == other); }
        const value_type operator*() const { return {*current_key_, *current_val_}; }

    private: // members

        key_iterator current_key_;
        value_iterator current_val_;
    };
};

//----------------------------------------------------------------------------------------------------------------------
}