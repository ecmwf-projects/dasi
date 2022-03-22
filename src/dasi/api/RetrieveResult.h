
#pragma once

#include "dasi/api/SplitKey.h"
#include "dasi/api/ObjectLocation.h"

#include <vector>

namespace dasi::api {

class ReadHandle;

//----------------------------------------------------------------------------------------------------------------------

class RetrieveResult {

public: // types

    using value_type = std::pair<SplitKey, ObjectLocation>;
    using const_iterator = std::vector<value_type>::const_iterator;

public: // methods

    void append(SplitKey&& key, ObjectLocation&& loc);

    const_iterator begin() const { return values_.begin(); }
    const_iterator end() const { return values_.end(); }

    ReadHandle* toHandle();

private: // members

    std::vector<value_type> values_;

};

//----------------------------------------------------------------------------------------------------------------------
}