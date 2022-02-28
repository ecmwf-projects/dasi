
#pragma once

#include <string>

namespace dasi::api {

class ReadHandle;

//----------------------------------------------------------------------------------------------------------------------

class ObjectLocation {

public: // types

    using offset_type = long long;
    using length_type = unsigned long long;

public: // methods

    ObjectLocation(const std::string& type, const std::string& location, offset_type offset, length_type length);
    virtual ~ObjectLocation();

    virtual const std::string& type() const;
    virtual const std::string& location() const;
    virtual offset_type offset() const;
    virtual length_type length() const;

    virtual ReadHandle* toHandle() const;

private: // members

    std::string type_;
    std::string location_;
    offset_type offset_;
    length_type length_;

};

//----------------------------------------------------------------------------------------------------------------------

}