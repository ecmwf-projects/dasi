
#include "dasi/api/Query.h"

#include "dasi/core/ContainerIostream.h"

namespace dasi {

//----------------------------------------------------------------------------------------------------------------------

Query::Query(std::initializer_list<std::pair<const std::string, std::vector<std::string>>> l) :
    values_(l) {}


void Query::print(std::ostream& s) const {
    s << values_;
}

//----------------------------------------------------------------------------------------------------------------------

}