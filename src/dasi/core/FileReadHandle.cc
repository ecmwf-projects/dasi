
#include "dasi/core/FileReadHandle.h"

#include <iostream>

namespace dasi::core {

FileReadHandle::FileReadHandle(const std::filesystem::path& path, long long offset) :
    path_(path), offset_(offset) {}

FileReadHandle::~FileReadHandle() {
    if (stream_.is_open()) {
        std::cerr << "ERROR: " << *this << " has not been closed" << std::endl;
    }
}

size_t FileReadHandle::read(void* buf, size_t len) {
    stream_.read(static_cast<char*>(buf), len);
    return stream_.gcount();
}

void FileReadHandle::open() {
    stream_.open(path_, std::ios::binary);
    stream_.seekg(offset_);
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
    return new FileReadHandle(location, offset);
}

api::ObjectLocation FileHandleBuilder::toLocation(const std::filesystem::path& path, long long offset) {
    return api::ObjectLocation{type, path, offset, 0};
}

FileHandleBuilder fileBuilder;

}
