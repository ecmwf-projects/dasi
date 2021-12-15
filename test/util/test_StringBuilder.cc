
#include "dasi/util/Test.h"

#include "dasi/util/StringBuilder.h"


CASE("construct some strings") {
    EXPECT(dasi::util::StringBuilder().str().empty());
    EXPECT((dasi::util::StringBuilder() << "Hello " << "there " << 1234).str() == "Hello there 1234");
}

int main(int argc, char** argv) {
    return ::dasi::util::run_tests();
}