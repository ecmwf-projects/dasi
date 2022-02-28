
#include "dasi/api/RetrieveResult.h"

#include "dasi/api/ReadHandle.h"

#include "dasi/core/AggregatedReadHandle.h"

namespace dasi::api {

//----------------------------------------------------------------------------------------------------------------------

void RetrieveResult::append(Key&& key, ObjectLocation&& loc) {
    values_.emplace_back(std::move(key), std::move(loc));
}

ReadHandle* RetrieveResult::toHandle() {
    std::vector<ReadHandle*> handles;
    handles.reserve(values_.size());
    for (auto& [key, loc] : values_) {
        handles.push_back(loc.toHandle());
    }
    return new dasi::core::AggregatedReadHandle(handles);
}

//----------------------------------------------------------------------------------------------------------------------

}