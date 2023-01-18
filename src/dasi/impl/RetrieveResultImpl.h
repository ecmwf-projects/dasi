
/// @author Simon Smart
/// @date   Jan 2023

#pragma once

#include "fdb5/api/helpers/ListIterator.h"

#include <memory>

namespace eckit { class DataHandle; }


namespace dasi {

//----------------------------------------------------------------------------------------------------------------------

class RetrieveResultImpl {

public: // methods

    explicit RetrieveResultImpl(fdb5::ListIterator&& iter);
    ~RetrieveResultImpl();

    [[nodiscard]]
    std::unique_ptr<eckit::DataHandle> dataHandle() const;

    /// The number of objects to be returned in this request
    size_t count() const;

private: // members

    /// @todo - use something other than ListElement, such that it can be properly iterated by the user-facing code
    std::vector<fdb5::ListElement> values_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace dasi
