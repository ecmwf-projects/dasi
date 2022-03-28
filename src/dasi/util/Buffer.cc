#include "dasi/util/Buffer.h"
#include "dasi/util/Exceptions.h"

#include <cstring>


namespace dasi::util {

//----------------------------------------------------------------------------------------------------------------------

namespace {

static char* allocate(size_t size) {
    return new char[size];
}

static void deallocate(char* buffer) {
    delete[] buffer;
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

Buffer::Buffer(size_t len) :
    size_(len) {
    create();
}

Buffer::Buffer(const void* p, size_t len) :
    size_(len) {
    create();
    copy(p, len);
}

Buffer::Buffer(Buffer&& rhs) noexcept :
    buffer_{rhs.buffer_},
    size_{rhs.size_} {
    rhs.buffer_ = nullptr;
    rhs.size_   = 0;
}

Buffer& Buffer::operator=(Buffer&& rhs) noexcept {
    if (this == &rhs) {
        return *this;
    }

    deallocate(buffer_);

    buffer_ = rhs.buffer_;
    size_   = rhs.size_;

    rhs.buffer_ = nullptr;
    rhs.size_   = 0;

    return *this;
}

Buffer::~Buffer() {
    destroy();
}

void Buffer::create() {
    buffer_ = allocate(size_);
}

void Buffer::destroy() {
    if (buffer_) {
        deallocate(buffer_);
        buffer_ = nullptr;
        size_   = 0;
    }
}

void Buffer::copy(const void* p, size_t size, size_t pos) {
    ASSERT(buffer_ && size_ >= pos + size);
    if (size) {
        ::memcpy(buffer_ + pos, p, size);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace dasi
