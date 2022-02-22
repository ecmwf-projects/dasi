
#include "dasi/core/AggregatedHandle.h"

#include "dasi/util/Exceptions.h"

#include <ostream>

namespace dasi::core {

AggregatedHandle::AggregatedHandle(const std::vector<Handle*>& handles) :
    handles_(handles.begin(), handles.end()),
    current_(handles_.begin()) {}

AggregatedHandle::~AggregatedHandle() {
    close();
}

size_t AggregatedHandle::read(void* buf, size_t len, bool stream) {

    if (current_ == handles_.end()) {
        return 0;
    }

    size_t nread = 0;
    auto cur = static_cast<char*>(buf);
    
    do {
        size_t chunk = (*current_)->read(static_cast<void*>(cur), len - nread, stream);
        cur += chunk;
        nread += chunk;

        if (stream && nread < len) {
            if (!next()) {
                return nread;
            }
        }
    } while (stream && nread < len);

    return nread;
}

const api::Key& AggregatedHandle::currentKey() const {
    ASSERT(current_ != handles_.end());
    return (*current_)->currentKey();
}

bool AggregatedHandle::next() {
    ++current_;
    bool moreData = current_ != handles_.end();
    if (moreData) {
        open();
    }
    return moreData;
}

void AggregatedHandle::open() {
    (*current_)->open();
}

void AggregatedHandle::close() {
    for (auto& h : handles_) {
        h->close();
    }

    handles_.clear();
}

void AggregatedHandle::print(std::ostream& s) const {
    s << "AggregatedHandle";
}

}