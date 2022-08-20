
#pragma once

#include "dasi/api/Key.h"
#include "dasi/api/ObjectLocation.h"
#include "dasi/api/ReadHandle.h"

#include <filesystem>
#include <fstream>


namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

class FileReadHandle : public api::ReadHandle {

public: // members

    constexpr static const char* type = "file";

public: // methods

    FileReadHandle(const std::filesystem::path& path, long long offset = 0, long long length = -1);
    ~FileReadHandle();

    size_t read(void* buf, size_t len) override;

    void open() override;
    void close() override;

    api::ObjectLocation toLocation() const;

private: // methods

    void print(std::ostream& s) const override;

private: // members

    std::filesystem::path path_;
    long long offset_;
    long long length_;
    long long pos_;
    std::ifstream stream_;
};

//----------------------------------------------------------------------------------------------------------------------

}
