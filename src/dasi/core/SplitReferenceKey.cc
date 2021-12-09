
#include "dasi/core/SplitReferenceKey.h"

#include "dasi/util/ContainerIostream.h"

namespace dasi {

//----------------------------------------------------------------------------------------------------------------------

void SplitReferenceKey::print(std::ostream& s) const {
    s << keys_;
}

//----------------------------------------------------------------------------------------------------------------------

}