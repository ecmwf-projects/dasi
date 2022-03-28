#pragma once

///@note Modelled on eckit/io/Buffer.h

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Simon Smart
/// @date   July 1996

#include <cstddef>


namespace dasi::util {

//----------------------------------------------------------------------------------------------------------------------

class Buffer  {

public:  // methods

    /// Allocate given length in bytes
    Buffer(size_t len);
    /// Allocate and copy memory of given length in bytes
    Buffer(const void*, size_t len);

    Buffer(Buffer&& rhs) noexcept;
    Buffer& operator=(Buffer&& rhs) noexcept;

    ~Buffer();

    void* data() { return buffer_; }
    const void* data() const { return buffer_; }

    /// @return allocated size
    size_t size() const { return size_; }

    /// Copy data of given size (bytes) into buffer at given position
    /// @pre Buffer must have sufficient size
    void copy(const void*, size_t size, size_t pos = 0);

protected:  // methods

    void create();
    void destroy();

private:  // members
    char* buffer_ = nullptr;
    size_t size_ = 0;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace dasi::util
