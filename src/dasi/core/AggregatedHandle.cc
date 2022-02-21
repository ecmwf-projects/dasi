
#include "dasi/core/AggregatedHandle.h"

#include "dasi/util/Exceptions.h"

#include <ostream>

namespace dasi::core {

AggregatedHandle::AggregatedHandle(const HandleList& handles) :
    handles_(handles),
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
    return current_ != handles_.end();
}

void AggregatedHandle::close() {
    for (auto& h : handles_) {
        h->close();
        delete h;
    }

    handles_.clear();
}

void AggregatedHandle::print(std::ostream& s) const {
    s << "AggregatedHandle";
}

}