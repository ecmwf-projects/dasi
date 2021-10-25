#include <iostream>

#include "dasi/api/Key.h"
#include "dasi/api/Query.h"
#include "dasi/api/Schema.h"

using namespace dasi;

int main() {

    Key k {
        {"level1a", "value"},
        {"level1b", "value2"},
        {"level2a", "value"},
        {"level2b", "value2"},
        {"level3a", "value"},
        {"level3b", "value2"}
    };

    std::cout << "key: " << k << std::endl;

    Query q {
        {"key", {"value1", "value2"}},
        {"key2", {"value1", "value2"}},
    };

    std::cout << "query: " << q << std::endl;

    SchemaRule3 r3 {"level3a", "level3b"};
    std::cout << "rule3: " << r3 << std::endl;

    SchemaRule2 r2 {{"level2a", "level2b"},
                    {{"level3a", "level3b"},
                     {"level3a", "level3b"}}};
    std::cout << "rule2: " << r2 << std::endl;

    SchemaRule1 r1 {
        {"level1a", "level1b"},
        {   {{"level2a", "level2b"},
             {   {"level3a", "level3b"},
                 {"level3a", "level3b"},
             }},
            {{"level2a", "level2b"},
             {   {"level3a", "level3b"},
                 {"level3a", "level3b"},
             }},
        }
    };

    std::cout << "rule1: " << r1 << std::endl;

//    class PrintVisitor : public SchemaVisitor {

//    };

    //Schema s {
    //    {{"L1a", "L1b"}
    //        {},
    //        {},
    //    },
    //    {{"level1a", "level1b"},
    //        {},
    //        {},
    //    },
    //};
    //std::cout << "schema: " << s << std::endl;
//  //  PrintVisitor visitor;
//    s.walk(k, visitor);

    return 0
;
}
