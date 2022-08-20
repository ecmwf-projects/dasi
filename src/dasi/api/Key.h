
#pragma once

#include "dasi/core/KeyT.h"
#include "dasi/util/ContainerIostream.h"

#include <map>

namespace dasi::api {

//----------------------------------------------------------------------------------------------------------------------

// Avoid compiler unhappiness on variable number of template arguments to std::map<>. Enforce 3.
template <typename K, typename T, typename C> using KeyMap = std::map<K, T, C>;

using Key = core::KeyT<KeyMap, std::string>;

//----------------------------------------------------------------------------------------------------------------------

}