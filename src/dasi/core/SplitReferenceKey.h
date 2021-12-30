
#pragma once

#include "dasi/core/SplitKeyT.h"
#include "dasi/core/OrderedReferenceKey.h"

namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

class SplitReferenceKey : public SplitKeyT<OrderedReferenceKey> {
public:
    using SplitKeyT::SplitKeyT;
};

//----------------------------------------------------------------------------------------------------------------------

}
