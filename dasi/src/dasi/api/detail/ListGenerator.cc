
#include "dasi/api/detail/ListGenerator.h"
#include "dasi/api/detail/ListGeneratorImpl.h"

namespace dasi {

//-------------------------------------------------------------------------------------------------

void ListElement::print(std::ostream& s) const {
    s << key;
}

//-------------------------------------------------------------------------------------------------

ListGenerator::ListGenerator(std::unique_ptr<ListGeneratorImpl>&& impl) :
    impl_(std::move(impl)) {}

ListGenerator::~ListGenerator() {}

//-------------------------------------------------------------------------------------------------

ListGeneratorIterator::ListGeneratorIterator(ListGeneratorImpl& generator) :
    generator_(generator) {}

bool ListGeneratorIterator::done() const {
    return generator_.done();
}

ListGeneratorIterator& ListGeneratorIterator::operator++() {
    generator_.next();
    return *this;
}

const ListElement& ListGeneratorIterator::operator*() const {
    return generator_.value();
}

//-------------------------------------------------------------------------------------------------

} // namespace dasi

