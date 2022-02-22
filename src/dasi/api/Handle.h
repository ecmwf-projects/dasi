
#pragma once

#include <cstddef>
#include <iosfwd>

#include "dasi/api/Key.h"


namespace dasi::api {

//----------------------------------------------------------------------------------------------------------------------

class Handle {

public: // methods

    virtual ~Handle() {}

    virtual size_t read(void* buf, size_t len, bool stream = false) = 0;
    virtual const Key& currentKey() const = 0;
    virtual bool next() { return false; }

    virtual void open() = 0;
    virtual void close() = 0;

private: // methods

    virtual void print(std::ostream& s) const = 0;

public: // friends

    friend std::ostream& operator<<(std::ostream& s, const Handle& handle) {
        handle.print(s);
        return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}