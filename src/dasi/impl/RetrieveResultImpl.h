
/// @author Simon Smart
/// @date   Jan 2023

#pragma once

#include "fdb5/api/helpers/ListIterator.h"

#include <memory>

namespace eckit { class DataHandle; }


namespace dasi {

//----------------------------------------------------------------------------------------------------------------------

class RetrieveResultImpl {

public:
    using value_type = std::vector<fdb5::ListElement>;

public: // methods

    explicit RetrieveResultImpl(RetrieveResultImpl& rhs);

    explicit RetrieveResultImpl(fdb5::ListIterator&& iter);
    ~RetrieveResultImpl();

    value_type::const_iterator begin() const;
    value_type::const_iterator end() const;

    [[nodiscard]]
    std::unique_ptr<eckit::DataHandle> dataHandle() const;

    /// The number of objects to be returned in this request
    size_t count() const;

private: // members

    /// @todo - use something other than ListElement, such that it can be properly iterated by the user-facing code
    value_type values_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace dasi
