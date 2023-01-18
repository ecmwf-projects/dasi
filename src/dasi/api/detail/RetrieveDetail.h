
/// @author Simon Smart
/// @date   Jan 2023

#pragma once


#include "eckit/io/DataHandle.h"

#include <memory>

namespace dasi {

//----------------------------------------------------------------------------------------------------------------------

class RetrieveResultImpl;

class RetrieveResult {

public: // methods

    explicit RetrieveResult(std::unique_ptr<RetrieveResultImpl>&& impl);
    ~RetrieveResult();

    [[ nodiscard ]]
    std::unique_ptr<eckit::DataHandle> dataHandle() const;

    [[ nodiscard ]]
    size_t count() const;

private: // members

    friend std::ostream& operator<<(std::ostream& s, const RetrieveResult& rr) {
        rr.print(s);
        return s;
    };

    void print(std::ostream& s) const;

private: // members

    std::unique_ptr<RetrieveResultImpl> impl_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace dasi

