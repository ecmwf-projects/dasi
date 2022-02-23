
#include "dasi/api/Result.h"

#include "dasi/core/AggregatedReadHandle.h"

namespace dasi::api {

//----------------------------------------------------------------------------------------------------------------------

Result::Result(std::vector<Key>&& keys, std::vector<item_type>&& values) :
    keys_(std::move(keys)), values_(std::move(values)) {}

Result::~Result() {
    for (auto val : values_) {
        delete val;
    }
}

ReadHandle* Result::toHandle() {
    auto handle = new dasi::core::AggregatedReadHandle(values_);
    values_.clear();
    return handle;
}

//----------------------------------------------------------------------------------------------------------------------

}