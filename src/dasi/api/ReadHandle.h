
#pragma once

#include <cstddef>
#include <iosfwd>

#include "dasi/api/Key.h"


namespace dasi::api {

//----------------------------------------------------------------------------------------------------------------------

class ReadHandle {

public: // types

    using offset_type = long long;
    using length_type = long long;  // -1 means unlimited length, if applicable

public: // methods

    virtual ~ReadHandle() {}

    virtual size_t read(void* buf, size_t len) = 0;

    virtual void open() = 0;
    virtual void close() = 0;

private: // methods

    virtual void print(std::ostream& s) const = 0;

public: // friends

    friend std::ostream& operator<<(std::ostream& s, const ReadHandle& handle) {
        handle.print(s);
        return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}