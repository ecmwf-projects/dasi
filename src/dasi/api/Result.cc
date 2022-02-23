
#include "dasi/api/Result.h"

#include "dasi/core/AggregatedHandle.h"

namespace dasi::api {

//----------------------------------------------------------------------------------------------------------------------

Result::Result(std::vector<Key>&& keys, std::vector<item_type>&& values) :
    keys_(std::move(keys)), values_(std::move(values)) {}

Result::~Result() {
    for (auto val : values_) {
        delete val;
    }
}

Handle* Result::toHandle() {
    auto handle = new dasi::core::AggregatedHandle(values_);
    values_.clear();
    return handle;
}

//----------------------------------------------------------------------------------------------------------------------

}