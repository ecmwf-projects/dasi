
#include "dasi/util/Test.h"

#include "dasi/core/Catalogue.h"
#include "dasi/core/OrderedReferenceKey.h"
#include "dasi/api/Config.h"

//----------------------------------------------------------------------------------------------------------------------

namespace {

class TestCatalogue1 : public dasi::core::Catalogue {
public:
    constexpr static const char* name = "test_cat_1";
    using Catalogue::Catalogue;
    ~TestCatalogue1() override = default;
};

class TestCatalogue2r : public dasi::core::Catalogue {
public:
    using Catalogue::Catalogue;
    ~TestCatalogue2r() override = default;
};

class TestCatalogue2w : public dasi::core::Catalogue {
public:
    using Catalogue::Catalogue;
    ~TestCatalogue2w() override = default;
};

dasi::core::CatalogueBuilder<TestCatalogue1> builder_1;
dasi::core::CatalogueBuilder<TestCatalogue2r, TestCatalogue2w> builder_2("test_cat_2");

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

    dasi::core::OrderedReferenceKey k ({
        {"key1", "value1"},
        {"key2", "value2"},
        {"key3", "value3"},
    });

    dasi::api::Config cfg;

    EXPECT_THROWS_AS(factory.buildReader("invalid-catalogue", k, cfg), dasi::util::SeriousBug);
    EXPECT_THROWS_AS(factory.buildWriter("invalid-catalogue", k, cfg), dasi::util::SeriousBug);

    std::unique_ptr<dasi::core::Catalogue> c1a = factory.buildReader("test_cat_1", k, cfg);
    EXPECT(c1a);
    EXPECT(dynamic_cast<TestCatalogue1*>(c1a.get()) != nullptr);
    EXPECT(dynamic_cast<TestCatalogue2r*>(c1a.get()) == nullptr);
    EXPECT(dynamic_cast<TestCatalogue2w*>(c1a.get()) == nullptr);

    std::unique_ptr<dasi::core::Catalogue> c1aX = factory.buildReader("test_cat_1", k, cfg);
    EXPECT(c1aX);
    EXPECT(dynamic_cast<TestCatalogue1*>(c1aX.get()) != nullptr);
    EXPECT(dynamic_cast<TestCatalogue2r*>(c1aX.get()) == nullptr);
    EXPECT(dynamic_cast<TestCatalogue2w*>(c1aX.get()) == nullptr);
    EXPECT(c1a != c1aX);

    std::unique_ptr<dasi::core::Catalogue> c1b = factory.buildWriter("test_cat_1", k, cfg);
    EXPECT(c1b);
    EXPECT(c1a != c1b);
    EXPECT(c1aX != c1b);
    EXPECT(dynamic_cast<TestCatalogue1*>(c1b.get()) != nullptr);
    EXPECT(dynamic_cast<TestCatalogue2r*>(c1b.get()) == nullptr);
    EXPECT(dynamic_cast<TestCatalogue2w*>(c1b.get()) == nullptr);

    std::unique_ptr<dasi::core::Catalogue> c2a = factory.buildReader("test_cat_2", k, cfg);
    EXPECT(c2a);
    EXPECT(c1a != c2a);
    EXPECT(c1aX != c2a);
    EXPECT(c1b != c2a);
    EXPECT(dynamic_cast<TestCatalogue1*>(c2a.get()) == nullptr);
    EXPECT(dynamic_cast<TestCatalogue2r*>(c2a.get()) != nullptr);
    EXPECT(dynamic_cast<TestCatalogue2w*>(c2a.get()) == nullptr);

    std::unique_ptr<dasi::core::Catalogue> c2b = factory.buildWriter("test_cat_2", k, cfg);
    EXPECT(c2b);
    EXPECT(c1a != c2b);
    EXPECT(c1aX != c2b);
    EXPECT(c1b != c2b);
    EXPECT(c2a != c2b);
    EXPECT(dynamic_cast<TestCatalogue1*>(c2b.get()) == nullptr);
    EXPECT(dynamic_cast<TestCatalogue2r*>(c2b.get()) == nullptr);
    EXPECT(dynamic_cast<TestCatalogue2w*>(c2b.get()) != nullptr);
}

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return ::dasi::util::run_tests();
}