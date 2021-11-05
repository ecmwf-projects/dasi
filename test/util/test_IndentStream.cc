
#include "dasi/util/Test.h"

#include "dasi/util/IndentStream.h"

#include <sstream>

//----------------------------------------------------------------------------------------------------------------------

CASE("Indent null string") {
    std::ostringstream ss;
    ss << "abcd";
    {
        ::dasi::IndentStream indent(ss, "XYZ");
    }
    ss << "efgh";
    EXPECT(ss.str() == "abcdefgh");
}

CASE("Indent endl vs \\n") {
    std::ostringstream ss;
    ss << "abcd";
    {
        ::dasi::IndentStream indent(ss, "XYZ");
        ss << "test1\n";
        ss << "test2" << std::endl;
        ss << "test3\n";
    }
    ss << "efgh";
    EXPECT(ss.str() == "abcdtest1\nXYZtest2\nXYZtest3\nefgh");
}

CASE("Indent firstline") {
    std::ostringstream ss;
    ss << "abcd";
    {
        ::dasi::IndentStream indent(ss, "XYZ", true);
        ss << "test1\n";
        ss << "test2" << std::endl;
        ss << "test3\n";
    }
    ss << "efgh";
    EXPECT(ss.str() == "abcdXYZtest1\nXYZtest2\nXYZtest3\nefgh");
}

CASE("Nested indenters") {
    std::ostringstream ss;
    ss << "abcd";
    {
        ::dasi::IndentStream indent(ss, "XYZ");
        ss << "test1\n";
        {
            ::dasi::IndentStream indent(ss, "999");
            ss << "test2" << std::endl;
            ss << "test3\n";
        }
        ss << "test4\n";
    }
    ss << "efgh";
    EXPECT(ss.str() == "abcdtest1\nXYZtest2\nXYZ999test3\nXYZtest4\nefgh");
}



//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return ::dasi::run_tests();
}