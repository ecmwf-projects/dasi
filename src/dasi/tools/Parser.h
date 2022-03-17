
#pragma once

#include <string>

#include "dasi/api/Key.h"
#include "dasi/api/Query.h"

namespace dasi::tools {

dasi::api::Key parseKey(const std::string& s);

dasi::api::Query parseQuery(const std::string& s);

}