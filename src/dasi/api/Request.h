
#pragma once

#include <initializer_list>
#include <iosfwd>
#include <map>
#include <string>
#include <vector>

namespace dasi {

//----------------------------------------------------------------------------------------------------------------------

class Request {

public: // methods

    Request() = default;
    Request(std::initializer_list<std::pair<const std::string, std::vector<std::string>>>);

    [[nodiscard]]
    bool has(const std::string_view& name) const;

    void set(const std::string& k, std::initializer_list<std::string> v);

private: // methods

    void print(std::ostream& s) const;

private: // friends

    friend std::ostream& operator<<(std::ostream& s, const Request& k) {
        k.print(s);
        return s;
    }

private: // members

    // n.b. use of transparent comparator --> allow lookup with std::string_view as key
    std::map<std::string, std::vector<std::string>, std::less<>> values_;
};

//----------------------------------------------------------------------------------------------------------------------

}