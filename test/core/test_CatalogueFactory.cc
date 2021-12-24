
#include "dasi/util/Test.h"

#include "dasi/core/Catalogue.h"

//----------------------------------------------------------------------------------------------------------------------

namespace {

class TestCatalogue1 : public dasi::core::Catalogue {
public:
    constexpr static const char* name = "test_cat_1";
    ~TestCatalogue1() override = default;
};

class TestCatalogue2 : public dasi::core::Catalogue {
public:
    constexpr static const char* name = "test_cat_2";
    ~TestCatalogue2() override = default;
};

dasi::core::CatalogueBuilder<TestCatalogue1> builder_1;
dasi::core::CatalogueBuilder<TestCatalogue2> builder_2;

}
//----------------------------------------------------------------------------------------------------------------------

CASE("List catalogues") {

    std::ostringstream ss;
    dasi::core::CatalogueFactory::instance().list(ss);

    EXPECT(ss.str().find("test_cat_1") != std::string::npos);
    EXPECT(ss.str().find("test_cat_2") != std::string::npos);
}

CASE("Build specified catalogues") {

    auto& factory(dasi::core::CatalogueFactory::instance());

    EXPECT_THROWS_AS(factory.build("invalid-catalogue"), dasi::util::SeriousBug);

    dasi::core::Catalogue* c1a = factory.build("test_cat_1");
    EXPECT(c1a != nullptr);
    EXPECT(dynamic_cast<TestCatalogue1*>(c1a) != nullptr);
    EXPECT(dynamic_cast<TestCatalogue2*>(c1a) == nullptr);

    dasi::core::Catalogue* c1b = factory.build("test_cat_1");
    EXPECT(c1b != nullptr);
    EXPECT(c1a != c1b);
    EXPECT(dynamic_cast<TestCatalogue1*>(c1b) != nullptr);
    EXPECT(dynamic_cast<TestCatalogue2*>(c1b) == nullptr);

    dasi::core::Catalogue* c2 = factory.build("test_cat_2");
    EXPECT(c2 != nullptr);
    EXPECT(c2 != c1a);
    EXPECT(c2 != c1b);
    EXPECT(dynamic_cast<TestCatalogue1*>(c2) == nullptr);
    EXPECT(dynamic_cast<TestCatalogue2*>(c2) != nullptr);
}

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return ::dasi::util::run_tests();
}