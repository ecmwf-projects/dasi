
#pragma once

namespace dasi::util {

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