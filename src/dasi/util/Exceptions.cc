
#include "dasi/util/Exceptions.h"

#include <iostream>
#include <sstream>
#include <cstring>

using namespace std::string_literals;

namespace dasi::util {

//----------------------------------------------------------------------------------------------------------------------

std::string CodeLocation::asString() const {
    std::ostringstream ss;
    print(ss);
    return ss.str();
}

void CodeLocation::print(std::ostream& os) const {
    if (file_) {
        os << " (" << file_ << " +" << line_;
        if (func_ && ::strlen(func_))
            os << " " << func_;
        os << ")";
    }
}

Exception::Exception(const std::string_view& what, const CodeLocation& loc) :
    what_(what),
    location_(loc) {}

Exception::Exception(std::string&& what, const CodeLocation& loc) :
    what_(std::move(what)),
    location_(loc) {}

void Exception::print(std::ostream& out) const {
    out << what_;
}

//----------------------------------------------------------------------------------------------------------------------

AssertionFailed::AssertionFailed(const std::string_view& msg, const CodeLocation& loc) :
    Exception("Assertion failed: "s += msg, loc) {}

SeriousBug::SeriousBug(const std::string_view& msg, const CodeLocation& loc) :
    Exception("Serious bug: "s += msg, loc) {}

NotImplemented::NotImplemented(const std::string_view& msg, const CodeLocation& loc) :
    Exception("Not implemented: "s += msg, loc) {}

NotImplemented::NotImplemented(const CodeLocation& loc) :
    Exception("Not Implemented"s, loc) {}

BadValue::BadValue(const std::string_view& msg, const CodeLocation& loc) :
    Exception("Bad value: "s += msg, loc) {}

KeyError::KeyError(const std::string_view& msg, const CodeLocation& loc) :
    Exception("Key error: "s += msg, loc) {}

UserError::UserError(const std::string_view& msg, const CodeLocation& loc) :
    Exception("User error: "s += msg, loc) {}

InvalidConfiguration::InvalidConfiguration(const std::string_view& msg, const CodeLocation& loc) :
    UserError("Invalid configuration: "s += msg, loc) {}

ObjectNotFound::ObjectNotFound(const std::string_view& msg, const CodeLocation& loc) :
    UserError("Object not found: "s += msg, loc) {}

//----------------------------------------------------------------------------------------------------------------------

} // namespace dasi
