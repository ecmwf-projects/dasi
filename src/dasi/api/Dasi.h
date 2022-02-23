
#pragma once

#include "dasi/api/Key.h"
#include "dasi/api/Config.h"
#include "dasi/api/Query.h"
#include "dasi/api/Result.h"

#include <iosfwd>
#include <memory>

namespace YAML {
class Node;
}

namespace dasi::core {
class Archiver;
class Retriever;
class Schema;
}

namespace dasi::api {

//----------------------------------------------------------------------------------------------------------------------

class Dasi {

public: // methods

    explicit Dasi(std::istream& config);
    explicit Dasi(const char* config);
    ~Dasi();

    void archive(const Key& key, const void* data, size_t length);
    Result retrieve(const Query& query);

private: // methods

    core::Schema& schema();
    core::Archiver& archiver();
    core::Retriever& retriever();

private: // members

    Config config_;

    std::unique_ptr<core::Schema> schema_;
    std::unique_ptr<core::Archiver> archiver_;
    std::unique_ptr<core::Retriever> retriever_;
};

//----------------------------------------------------------------------------------------------------------------------

}