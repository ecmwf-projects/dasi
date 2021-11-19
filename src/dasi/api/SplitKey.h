
#pragma once

#include <string>
#include <iosfwd>
#include <array>

#include "dasi/api/Key.h"

namespace dasi {

//----------------------------------------------------------------------------------------------------------------------

class SplitKey {

public: // methods

    SplitKey() = default;

private: // methods

    void print(std::ostream& s) const;

private: // friends

    friend std::ostream& operator<<(std::ostream& s, const SplitKey& k) {
        k.print(s);
        return s;
    }

private: // members

    std::array<Key, 3> keys_;
};

//----------------------------------------------------------------------------------------------------------------------

}