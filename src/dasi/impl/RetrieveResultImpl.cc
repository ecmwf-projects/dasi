
#include "dasi/impl/RetrieveResultImpl.h"

#include "eckit/io/DataHandle.h"
#include "fdb5/io/HandleGatherer.h"

#include <ostream>

namespace dasi {

//----------------------------------------------------------------------------------------------------------------------


RetrieveResultImpl::RetrieveResultImpl(RetrieveResultImpl& rhs) :
    values_(rhs.values_) {}

RetrieveResultImpl::RetrieveResultImpl(fdb5::ListIterator&& iter) {
    fdb5::ListElement elem;
    while (iter.next(elem)) {
        values_.push_back(elem);
    }
}

RetrieveResultImpl::~RetrieveResultImpl() {}

RetrieveResultImpl::value_type::const_iterator RetrieveResultImpl::begin() const {
    return values_.begin();
}

RetrieveResultImpl::value_type::const_iterator RetrieveResultImpl::end() const {
    return values_.end();
}

std::unique_ptr<eckit::DataHandle> RetrieveResultImpl::dataHandle() const {

    /// @todo - discuss optimise/sorted
    bool sorted = false;
    fdb5::HandleGatherer result(sorted);

    for (const auto& elem : values_) {
        result.add(elem.location().dataHandle());
    }

    return std::unique_ptr<eckit::DataHandle>{result.dataHandle()};
}

size_t RetrieveResultImpl::count() const {
    return values_.size();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace dasi
