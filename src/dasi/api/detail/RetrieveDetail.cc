
#include "dasi/api/detail/RetrieveDetail.h"

#include "dasi/impl/RetrieveResultImpl.h"

#include <ostream>

namespace dasi {

//----------------------------------------------------------------------------------------------------------------------

RetrieveResult::RetrieveResult(std::unique_ptr<RetrieveResultImpl>&& impl) :
    impl_(std::move(impl)) {}

RetrieveResult::RetrieveResult(RetrieveResult&& rhs) noexcept : impl_(std::move(rhs.impl_)) {}

RetrieveResult& RetrieveResult::operator=(RetrieveResult&& rhs) noexcept {
    impl_ = std::move(rhs.impl_);
    return *this;
}

RetrieveResult::~RetrieveResult() {}

std::unique_ptr<eckit::DataHandle> RetrieveResult::dataHandle() const {
    return impl_->dataHandle();
}

size_t RetrieveResult::count() const {
    return impl_->count();
}

void RetrieveResult::print(std::ostream& s) const {
    s << "RetrieveResult()";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace dasi
