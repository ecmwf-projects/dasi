
#include "dasi/core/AggregatedHandle.h"

#include "dasi/util/Exceptions.h"

#include <ostream>

namespace dasi::core {

AggregatedHandle::AggregatedHandle(const HandleList& handles) :
    handles_(handles),
    current_(handles_.begin()) {}

size_t AggregatedHandle::read(void* buf, size_t len, bool stream) {

    if (current_ == handles_.end()) {
        return 0;
    }

    size_t read = 0;
    auto cur = static_cast<char*>(buf);
    
    do {
        size_t chunk = (*current_)->read(static_cast<void*>(cur), len - read, stream);
        cur += chunk;
        read += chunk;

        if (stream && read < len) {
            if (!next()) {
                return read;
            }
        }
    } while (stream && read < len);

    return read;
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
    }

    handles_.clear();
}

void AggregatedHandle::print(std::ostream& s) const {
    s << "AggregatedHandle";
}

}