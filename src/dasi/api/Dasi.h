
#pragma once

#include "dasi/api/Key.h"

#include <iosfwd>
#include <memory>

namespace YAML {
class Node;
}

namespace dasi::core {
class Archiver;
class Schema;
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

    core::Schema& schema();
    core::Archiver& archiver();

private: // members

    // This is only a unique_ptr so that we don't have to include yaml-cpp/yaml.h here
    // We don't want that to be a required part of the API. Should be internal to the library.
    std::unique_ptr<YAML::Node> config_;

    std::unique_ptr<core::Schema> schema_;
    std::unique_ptr<core::Archiver> archiver_;
};

//----------------------------------------------------------------------------------------------------------------------

}