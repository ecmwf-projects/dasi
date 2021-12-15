
#pragma once

#include "dasi/core/Schema.h"

#include <iosfwd>
#include <memory>


namespace YAML {
    class Node;
}

namespace dasi::core {
class Archiver;
}

namespace dasi::api {

//----------------------------------------------------------------------------------------------------------------------

class Dasi {

public: // methods

    explicit Dasi(std::istream& config);
    explicit Dasi(const char* config);
    ~Dasi();

    void archive(Key& key, const void* data, size_t length);

private: // methods

    Dasi(const YAML::Node& config);

    core::Archiver& archiver();

private: // members

    core::Schema schema_;
    std::unique_ptr<core::Archiver> archiver_;
};

//----------------------------------------------------------------------------------------------------------------------

}