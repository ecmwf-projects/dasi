
#pragma once

#include "dasi/api/Key.h"

#include <memory>
#include <vector>

namespace dasi::api {

class ReadHandle;

//----------------------------------------------------------------------------------------------------------------------

class RetrieveResult {

public: // types

    using item_type = ReadHandle*;
    using value_type = std::pair<Key, item_type>;
    using const_iterator = std::vector<value_type>::const_iterator;

public: // methods

    ~RetrieveResult();

    void append(const Key& key, ReadHandle* value);

    const_iterator begin() const { return values_.begin(); }
    const_iterator end() const { return values_.end(); }

    ReadHandle* toHandle();

private: // members

    std::vector<value_type> values_;

};

//----------------------------------------------------------------------------------------------------------------------
}