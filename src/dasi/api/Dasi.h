
#pragma once

#include "dasi/api/Key.h"
#include "dasi/api/Config.h"

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

    core::Schema& schema();
    core::Archiver& archiver();

private: // members

    Config config_;

    std::unique_ptr<core::Schema> schema_;
    std::unique_ptr<core::Archiver> archiver_;
};

//----------------------------------------------------------------------------------------------------------------------

}