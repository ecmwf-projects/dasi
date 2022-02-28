
#include "dasi/api/ObjectLocation.h"

#include "dasi/core/HandleFactory.h"

namespace dasi::api {

//----------------------------------------------------------------------------------------------------------------------

ObjectLocation::ObjectLocation(const std::string& type, const std::string& location, offset_type offset, length_type length) :
    type_(type),
    location_(location),
    offset_(offset),
    length_(length) {}

ObjectLocation::~ObjectLocation() {}

const std::string& ObjectLocation::type() const {
    return type_;
}

const std::string& ObjectLocation::location() const {
    return location_;
}

ObjectLocation::offset_type ObjectLocation::offset() const {
    return offset_;
}

ObjectLocation::length_type ObjectLocation::length() const {
    return length_;
}

ReadHandle* ObjectLocation::toHandle() const {
    return core::HandleFactory::instance().buildReadHandle(type_, location_, offset_, length_);
}

//----------------------------------------------------------------------------------------------------------------------

}