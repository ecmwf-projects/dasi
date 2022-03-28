
#pragma once

#include <initializer_list>
#include <iosfwd>
#include <map>
#include <string>
#include <vector>

namespace dasi::api {

//----------------------------------------------------------------------------------------------------------------------

class Query {

public: // types

    using value_type = std::vector<std::string>;

public: // methods

    Query() = default;
    Query(std::initializer_list<std::pair<const std::string, value_type>>);

    [[nodiscard]]
    bool has(const std::string_view& name) const;

    void set(const std::string& k, std::initializer_list<std::string> v);
    void set(const std::string& k, const value_type& v);

    void append(const std::string& k, const std::string& v);

    const value_type& get(const std::string_view& name) const;

private: // methods

    void print(std::ostream& s) const;

private: // friends

    friend std::ostream& operator<<(std::ostream& s, const Query& k) {
        k.print(s);
        return s;
    }

private: // members

    // n.b. use of transparent comparator --> allow lookup with std::string_view as key
    std::map<std::string, value_type, std::less<>> values_;
};

//----------------------------------------------------------------------------------------------------------------------

}
