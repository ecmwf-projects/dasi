
#pragma once

#include <map>
#include <string>
#include <iosfwd>
#include <initializer_list>
#include <utility>

namespace dasi {

//----------------------------------------------------------------------------------------------------------------------

class Key {

public: // methods

    Key() = default;
    Key(std::initializer_list<std::pair<const std::string, std::string>>);

    [[nodiscard]]
    bool has(const std::string_view& name) const;

    void set(const std::string& k, const std::string_view& v);

private: // methods

    void print(std::ostream& s) const;

private: // friends

    friend std::ostream& operator<<(std::ostream& s, const Key& k) {
        k.print(s);
        return s;
    }

private: // members

    // n.b. use of transparent comparator --> allow lookup with std::string_view as key
    std::map<std::string, std::string, std::less<>> values_;
};

//----------------------------------------------------------------------------------------------------------------------

}