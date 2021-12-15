
#pragma once

#include <string>
#include <iosfwd>
#include <array>

#include "dasi/core/OrderedReferenceKey.h"

namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

class SplitReferenceKey {

public: // methods

    SplitReferenceKey() = default;

    OrderedReferenceKey& operator[](int level) { return keys_[level]; }
    const OrderedReferenceKey& operator[](int level) const { return keys_[level]; }

private: // methods

    void print(std::ostream& s) const;

private: // friends

    friend std::ostream& operator<<(std::ostream& s, const SplitReferenceKey& k) {
        k.print(s);
        return s;
    }

private: // members

    std::array<OrderedReferenceKey, 3> keys_;
};

//----------------------------------------------------------------------------------------------------------------------

}