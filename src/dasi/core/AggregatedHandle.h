
#pragma once

#include "dasi/api/Handle.h"

#include <vector>


namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

class AggregatedHandle : public api::Handle {

public: // types

    using HandleList = std::vector<Handle*>;

public: // methods

    AggregatedHandle(const HandleList& handles);

    size_t read(void* buf, size_t len, bool stream = false) override;
    const api::Key& currentKey() const override;
    bool next() override;

    void close() override;

private: // methods

    void print(std::ostream& s) const override;

private: // members

    HandleList handles_;
    HandleList::iterator current_;
};

//----------------------------------------------------------------------------------------------------------------------

}