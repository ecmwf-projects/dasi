
#pragma once

#include "dasi/api/Handle.h"

#include <memory>
#include <vector>


namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

class AggregatedHandle : public api::Handle {

private: // types

    using Item = std::unique_ptr<Handle>;
    using HandleList = std::vector<Item>;

public: // methods

    AggregatedHandle(const std::vector<Handle*>& handles);
    ~AggregatedHandle();

    size_t read(void* buf, size_t len, bool stream = false) override;
    bool next() override;

    void open() override;
    void close() override;

private: // methods

    void print(std::ostream& s) const override;

private: // members

    HandleList handles_;
    HandleList::iterator current_;
};

//----------------------------------------------------------------------------------------------------------------------

}