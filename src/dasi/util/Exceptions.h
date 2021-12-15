
// Modelled on eckit/exception/Exceptions.h

#pragma once

#include <iosfwd>
#include <exception>
#include <string>
#include <string_view>


namespace dasi::util {

//----------------------------------------------------------------------------------------------------------------------

class CodeLocation {

public:  // methods

    CodeLocation(const char* file, int line, const char* func) : line_(line), file_(file), func_(func) {}

    ~CodeLocation() = default;

    std::string asString() const;

protected:  // methods

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const CodeLocation& loc) {
        loc.print(s);
        return s;
    }

    int line() const { return line_; }

    const char* file() const { return file_; }

    const char* func() const { return func_; }

private:  // members
    int line_;
    const char* file_;
    const char* func_;
};

#define Here() ::dasi::util::CodeLocation(__FILE__, __LINE__, __func__)

//----------------------------------------------------------------------------------------------------------------------

class Exception : public std::exception {

public:  // methods

    Exception() = delete;

    Exception(const std::string_view& what, const CodeLocation& location);
    Exception(std::string&& what, const CodeLocation& location);

    ~Exception() noexcept override = default;

    const char* what() const noexcept override { return what_.c_str(); }

    const CodeLocation& location() const { return location_; }

protected: // methods

    virtual void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const Exception& p) {
        p.print(s);
        return s;
    }

private: // members

    std::string what_;
    CodeLocation location_;
};

//----------------------------------------------------------------------------------------------------------------------

struct SeriousBug : public Exception {
    SeriousBug(const std::string_view& msg, const CodeLocation& loc);
};

struct NotImplemented : public Exception {
    NotImplemented(const std::string_view& msg, const CodeLocation& loc);
    NotImplemented(const CodeLocation& loc);
};

struct AssertionFailed : public Exception {
    AssertionFailed(const std::string_view& msg, const CodeLocation& loc);
};

struct BadValue : public Exception {
    BadValue(const std::string_view& msg, const CodeLocation& loc);
};

struct KeyError : public Exception {
    KeyError(const std::string_view& msg, const CodeLocation& loc);
};

struct UserError : public Exception {
    UserError(const std::string_view& msg, const CodeLocation& loc);
};

struct InvalidConfiguration : public UserError {
    InvalidConfiguration(const std::string_view& msg, const CodeLocation& loc);
};

//----------------------------------------------------------------------------------------------------------------------

inline void Assert(int code, const std::string& msg, const char* file, int line, const char* func) {
    if (code != 0) {
        throw AssertionFailed(msg, CodeLocation(file, line, func));
    }
}

inline void Assert(int code, const char* msg, const char* file, int line, const char* func) {
    if (code != 0) {
        throw AssertionFailed(msg, CodeLocation(file, line, func));
    }
}

#define ASSERT(a) ::dasi::util::Assert(!(a), #a, __FILE__, __LINE__, __func__)
#define NOTIMP throw ::dasi::util::NotImplemented(Here());

//----------------------------------------------------------------------------------------------------------------------

} // namespace dasi
