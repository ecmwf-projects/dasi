
#pragma once

#include <sstream>
#include <string>


namespace dasi::util {

//----------------------------------------------------------------------------------------------------------------------

// A helper class for building strings on one line

class StringBuilder {

public: // methods

    template <typename T>
    StringBuilder& operator<< (const T& data) {
        ss_ << data;
        return *this;
    }

    // Explicitly convert to string
    std::string str() const {
        return ss_.str();
    }

    // Implicitly convert to string
    operator std::string() {
        return ss_.str();
    }

private: // members

    std::ostringstream ss_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace dasi
