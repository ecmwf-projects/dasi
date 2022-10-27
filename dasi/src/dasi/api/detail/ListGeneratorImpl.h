
/// @author Simon Smart
/// @date   Oct 2022

#pragma once

#include "fdb5/api/helpers/ListIterator.h"
#include "ListGenerator.h"

namespace dasi {

//-------------------------------------------------------------------------------------------------

class ListGeneratorImpl {

public: // methods

    explicit ListGeneratorImpl(fdb5::ListIterator&& iter);

    void next();

    [[ nodiscard ]]
    const dasi::ListElement& value() const;

    [[ nodiscard ]]
    bool done() const;

private: // members

    fdb5::ListElement fdb5Element_;
    dasi::ListElement dasiElement_;
    fdb5::ListIterator iter_;
    bool done_;
};

//-------------------------------------------------------------------------------------------------

} // namespace dasi
