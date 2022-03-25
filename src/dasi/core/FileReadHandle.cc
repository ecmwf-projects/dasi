
#include "dasi/core/FileReadHandle.h"

#include <iostream>

namespace dasi::core {

FileReadHandle::FileReadHandle(const std::filesystem::path& path, long long offset, long long length) :
    path_(path), offset_(offset), length_(length), pos_(0) {}

FileReadHandle::~FileReadHandle() {
    if (stream_.is_open()) {
        std::cerr << "ERROR: " << *this << " has not been closed" << std::endl;
    }
}

size_t FileReadHandle::read(void* buf, size_t len) {
    if (length_ != -1) {
        len = std::min(len, size_t(length_ - pos_));
    }
    stream_.read(static_cast<char*>(buf), len);
    size_t nread = stream_.gcount();
    pos_ += nread;
    return nread;
}

void FileReadHandle::open() {
    stream_.open(path_, std::ios::binary);
    stream_.seekg(offset_);
    pos_ = 0;
}

void FileReadHandle::close() {
    stream_.close();
}

void FileReadHandle::print(std::ostream& s) const {
    s << "FileReadHandle[" << path_ << "]";
}

//----------------------------------------------------------------------------------------------------------------------

FileHandleBuilder::FileHandleBuilder() : core::HandleBuilderBase(type) {}

api::ReadHandle* FileHandleBuilder::makeReadHandle(const std::string& location, offset_type offset, length_type length) {
    return new FileReadHandle(location, offset, length);
}

api::ObjectLocation FileHandleBuilder::toLocation(const std::filesystem::path& path, long long offset, long long length) {
    return api::ObjectLocation{type, path, offset, length};
}

FileHandleBuilder fileBuilder;

}
