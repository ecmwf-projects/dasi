
#include "dasi/core/SplitReferenceKey.h"

#include "dasi/util/ContainerIostream.h"

namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

void SplitReferenceKey::print(std::ostream& s) const {
    s << keys_;
}

//----------------------------------------------------------------------------------------------------------------------

}