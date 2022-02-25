
#include "dasi/api/RetrieveResult.h"

#include "dasi/core/AggregatedReadHandle.h"

namespace dasi::api {

//----------------------------------------------------------------------------------------------------------------------

RetrieveResult::RetrieveResult(std::vector<Key>&& keys, std::vector<item_type>&& values) :
    keys_(std::move(keys)), values_(std::move(values)) {}

RetrieveResult::~RetrieveResult() {
    for (auto val : values_) {
        delete val;
    }
}

ReadHandle* RetrieveResult::toHandle() {
    auto handle = new dasi::core::AggregatedReadHandle(values_);
    values_.clear();
    return handle;
}

//----------------------------------------------------------------------------------------------------------------------

}