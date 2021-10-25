
#pragma once

#include <map>
#include <vector>
#include <string>
#include <iosfwd>
#include <initializer_list>
#include <utility>

namespace dasi {

//----------------------------------------------------------------------------------------------------------------------

class Query {

public: // methods

    Query(std::initializer_list<std::pair<const std::string, std::vector<std::string>>>);

private: // methods

    void print(std::ostream& s) const;

private: // friends

    friend std::ostream& operator<<(std::ostream& s, const Query& q) {
        q.print(s);
        return s;
    }

private: // members

    std::map<std::string, std::vector<std::string>> values_;
};

//----------------------------------------------------------------------------------------------------------------------

}