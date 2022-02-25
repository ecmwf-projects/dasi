
#pragma once

#include <cstddef>
#include <iosfwd>

#include "dasi/api/Key.h"


namespace dasi::api {

//----------------------------------------------------------------------------------------------------------------------

class ReadHandle {

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

template <typename T>
class AutoCloser {

public: // methods

    AutoCloser(T& obj) : obj_(obj) {}

    ~AutoCloser() { obj_.close(); }

private: // members

    T& obj_;
};

//----------------------------------------------------------------------------------------------------------------------

}