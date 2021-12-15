
#pragma once

#include "dasi/util/OrderedMap.h"
#include "dasi/core/KeyT.h"

#include <string>

namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

// This class implements Key, but:
// i) Maintains order (as constructed by Schema)
// ii) Stores string_views, to avoid unecessary string copies

using OrderedReferenceKey = KeyT<util::OrderedMap, std::string_view>;

//----------------------------------------------------------------------------------------------------------------------

}
