
#pragma once

#include "dasi/api/Key.h"
#include "dasi/api/ObjectLocation.h"
#include "dasi/api/ReadHandle.h"

#include "dasi/core/HandleFactory.h"

#include <filesystem>
#include <fstream>


namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

class FileReadHandle : public api::ReadHandle {

public: // methods

    FileReadHandle(const std::filesystem::path& path, long long offset = 0, long long length = -1);
    ~FileReadHandle();

    size_t read(void* buf, size_t len) override;

    void open() override;
    void close() override;

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

class FileHandleBuilder : public HandleBuilderBase {

public: // members

    constexpr static const char* type = "file";

public: // methods

    FileHandleBuilder();

    api::ReadHandle* makeReadHandle(const std::string& location, offset_type offset, length_type length) override;

    static api::ObjectLocation toLocation(const std::filesystem::path& path, long long offset = 0, long long length = -1);

};

//----------------------------------------------------------------------------------------------------------------------

}
