
#pragma once

#include "dasi/api/ReadHandle.h"

#include "dasi/util/AutoCloser.h"

#include <memory>
#include <vector>


namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

class AggregatedReadHandle : public api::ReadHandle {

private: // types

    using Item = std::unique_ptr<ReadHandle>;
    using HandleList = std::vector<Item>;

public: // methods

    AggregatedReadHandle(const std::vector<ReadHandle*>& handles);
    ~AggregatedReadHandle();

    size_t read(void* buf, size_t len) override;

    void open() override;
    void close() override;

private: // methods

    void openInternal();

    void print(std::ostream& s) const override;

private: // members

    HandleList handles_;
    HandleList::iterator current_;
    std::unique_ptr<util::AutoCloser<ReadHandle>> closer_;
};

//----------------------------------------------------------------------------------------------------------------------

}