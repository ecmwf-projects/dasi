
#pragma once

#include "dasi/util/OrderedMap.h"
#include "dasi/core/KeyT.h"

#include <string>

namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

// This class implements Key, but:
// i) Maintains order (as constructed by Schema)
// ii) Stores string_views, to avoid unecessary string copies

template <typename K, typename T, typename C> using OrderedReferenceKeyMap = util::OrderedMap<K, T, C>;

class OrderedReferenceKey : public KeyT<OrderedReferenceKeyMap, std::string_view> {
public:
    using KeyT::KeyT;
};

//----------------------------------------------------------------------------------------------------------------------

}
