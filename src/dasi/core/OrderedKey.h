
#pragma once

#include "dasi/util/OrderedMap.h"

#include "dasi/core/KeyT.h"
#include "dasi/core/OrderedReferenceKey.h"

#include <string>

namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

// This class implements Key, but:
// i) Maintains order (as constructed by Schema)

template <typename K, typename T, typename C> using OrderedKeyMap = util::OrderedMap<K, T, C>;

class OrderedKey : public KeyT<OrderedKeyMap, std::string> {

public: // methods
    using KeyT::KeyT;

    OrderedKey(const OrderedReferenceKey& rhs) {
        for (const auto& kv : rhs) {
            set(kv.first, std::string(kv.second));
        }
    }

    OrderedKey& operator=(const OrderedReferenceKey& rhs) {
        clear();
        for (const auto& kv : rhs) {
            set(kv.first, std::string(kv.second));
        }
        return *this;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}
