
#pragma once

#include "dasi/core/Schema.h"

#include <iosfwd>

namespace YAML {
    class Node;
}

namespace dasi {

//----------------------------------------------------------------------------------------------------------------------

class Dasi {

public: // methods

    explicit Dasi(std::istream& config);
    explicit Dasi(const char* config);
    ~Dasi() = default;

    void archive(Key& key, const void* data, size_t length);

private: // methods

    Dasi(const YAML::Node& config);

private: // members

    Schema schema_;
};

//----------------------------------------------------------------------------------------------------------------------

}