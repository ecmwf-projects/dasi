
#include "dasi/util/Test.h"

#include "dasi/core/SchemaKeyIterator.h"
#include "dasi/api/Key.h"


using namespace std::string_literals;

CASE("Iterate list of keys") {

    std::vector<std::string> v1 {"aa", "bb", "cc"};
    std::vector<std::string> v2 {"dd", "ee", "ff"};
    std::vector<std::string> v3 {"gg", "hh", "ii"};
    std::vector<std::string> v4 {"jj", "kk", "ll"};
    std::vector<std::string> v5 {"mm", "nn", "oo"};
    std::vector<std::string> v6 {"pp", "qq", "rr"};

    std::vector<dasi::api::Key> seq1 {
        {{"k1", "aa"}, {"k2", "dd"}}, {{"k1", "bb"}, {"k2", "dd"}}, {{"k1", "cc"}, {"k2", "dd"}},
        {{"k1", "aa"}, {"k2", "ee"}}, {{"k1", "bb"}, {"k2", "ee"}}, {{"k1", "cc"}, {"k2", "ee"}},
        {{"k1", "aa"}, {"k2", "ff"}}, {{"k1", "bb"}, {"k2", "ff"}}, {{"k1", "cc"}, {"k2", "ff"}},
    };

    std::vector<dasi::api::Key> seq2 {
        {{"k3", "gg"}, {"k4", "jj"}}, {{"k3", "hh"}, {"k4", "jj"}}, {{"k3", "ii"}, {"k4", "jj"}},
        {{"k3", "gg"}, {"k4", "kk"}}, {{"k3", "hh"}, {"k4", "kk"}}, {{"k3", "ii"}, {"k4", "kk"}},
        {{"k3", "gg"}, {"k4", "ll"}}, {{"k3", "hh"}, {"k4", "ll"}}, {{"k3", "ii"}, {"k4", "ll"}},
    };

    std::vector<dasi::api::Key> seq3 {
        {{"k5", "mm"}, {"k6", "pp"}}, {{"k5", "nn"}, {"k6", "pp"}}, {{"k5", "oo"}, {"k6", "pp"}},
        {{"k5", "mm"}, {"k6", "qq"}}, {{"k5", "nn"}, {"k6", "qq"}}, {{"k5", "oo"}, {"k6", "qq"}},
        {{"k5", "mm"}, {"k6", "rr"}}, {{"k5", "nn"}, {"k6", "rr"}}, {{"k5", "oo"}, {"k6", "rr"}},
    };

    dasi::core::SchemaKeyIterator<decltype(v1)> ki;

    ki.insert(0, "k1", v1);
    ki.insert(0, "k2", v2);
    ki.insert(1, "k3", v3);
    ki.insert(1, "k4", v4);
    ki.insert(2, "k5", v5);
    ki.insert(2, "k6", v6);

    struct Visitor {
        int level1 = -1;
        int level2 = -1;
        int level3 = -1;
        int count = 0;
        const std::vector<dasi::api::Key>& seq1_;
        const std::vector<dasi::api::Key>& seq2_;
        const std::vector<dasi::api::Key>& seq3_;
        Visitor(const std::vector<dasi::api::Key>& seq1,
                const std::vector<dasi::api::Key>& seq2,
                const std::vector<dasi::api::Key>& seq3) :
                seq1_(seq1), seq2_(seq2), seq3_(seq3) {}
        void firstLevel(const dasi::core::SplitReferenceKey& key) {
            level1++;
            EXPECT(key[0] == seq1_[level1]);
            level2 = -1;
            level3 = -1;
        }
        void secondLevel(const dasi::core::SplitReferenceKey& key) {
            level2++;
            EXPECT(key[0] == seq1_[level1]);
            EXPECT(key[1] == seq2_[level2]);
            level3 = -1;
        }
        void thirdLevel(const dasi::core::SplitReferenceKey& key) {
            level3++;
            count++;
            EXPECT(key[0] == seq1_[level1]);
            EXPECT(key[1] == seq2_[level2]);
            EXPECT(key[2] == seq3_[level3]);
        }
    };

    Visitor v{seq1, seq2, seq3};
    ki.visit(v);
    EXPECT(v.count == (seq1.size() * seq2.size() * seq3.size()));
}

CASE("Can handle non-list iteration (i.e. key vs request)") {

    std::string v1 {"aa"};
    std::string v2 {"dd"};
    std::string v3 {"gg"};
    std::string v4 {"jj"};
    std::string v5 {"mm"};
    std::string v6 {"pp"};

    dasi::api::Key lev1 {{"k1", "aa"}, {"k2", "dd"}};
    dasi::api::Key lev2 {{"k3", "gg"}, {"k4", "jj"}};
    dasi::api::Key lev3 {{"k5", "mm"}, {"k6", "pp"}};

    dasi::core::SchemaKeyIterator<decltype(v1)> ki;

    ki.insert(0, "k1", v1);
    ki.insert(0, "k2", v2);
    ki.insert(1, "k3", v3);
    ki.insert(1, "k4", v4);
    ki.insert(2, "k5", v5);
    ki.insert(2, "k6", v6);

    struct Visitor {
        bool visited1 = false;
        bool visited2 = false;
        bool visited3 = false;
        int count = 0;
        const dasi::api::Key& lev1_;
        const dasi::api::Key& lev2_;
        const dasi::api::Key& lev3_;
        Visitor(const dasi::api::Key& lev1, const dasi::api::Key& lev2, const dasi::api::Key& lev3) :
            lev1_(lev1), lev2_(lev2), lev3_(lev3) {}
        void firstLevel(const dasi::core::SplitReferenceKey& key) {
            EXPECT(!visited1);
            EXPECT(key[0] == lev1_);
            visited1 = true;
        }
        void secondLevel(const dasi::core::SplitReferenceKey& key) {
            EXPECT(visited1);
            EXPECT(!visited2);
            EXPECT(key[0] == lev1_);
            EXPECT(key[1] == lev2_);
            visited2 = true;
        }
        void thirdLevel(const dasi::core::SplitReferenceKey& key) {
            EXPECT(visited1);
            EXPECT(visited2);
            EXPECT(!visited3);
            EXPECT(key[0] == lev1_);
            EXPECT(key[1] == lev2_);
            EXPECT(key[2] == lev3_);
            visited3 = true;
            count++;
        }
    };

    Visitor v{lev1, lev2, lev3};
    ki.visit(v);
    EXPECT(v.count == 1);
}

CASE("Vistor doesn't need to be complete") {


    std::vector<std::string> v1 {"aa", "bb", "cc"};
    std::vector<std::string> v2 {"dd", "ee", "ff"};
    std::vector<std::string> v3 {"gg", "hh", "ii"};
    std::vector<std::string> v4 {"jj", "kk", "ll"};
    std::vector<std::string> v5 {"mm", "nn", "oo"};
    std::vector<std::string> v6 {"pp", "qq", "rr"};

    std::vector<dasi::api::Key> seq1 {
        {{"k1", "aa"}, {"k2", "dd"}}, {{"k1", "bb"}, {"k2", "dd"}}, {{"k1", "cc"}, {"k2", "dd"}},
        {{"k1", "aa"}, {"k2", "ee"}}, {{"k1", "bb"}, {"k2", "ee"}}, {{"k1", "cc"}, {"k2", "ee"}},
        {{"k1", "aa"}, {"k2", "ff"}}, {{"k1", "bb"}, {"k2", "ff"}}, {{"k1", "cc"}, {"k2", "ff"}},
    };

    std::vector<dasi::api::Key> seq2 {
        {{"k3", "gg"}, {"k4", "jj"}}, {{"k3", "hh"}, {"k4", "jj"}}, {{"k3", "ii"}, {"k4", "jj"}},
        {{"k3", "gg"}, {"k4", "kk"}}, {{"k3", "hh"}, {"k4", "kk"}}, {{"k3", "ii"}, {"k4", "kk"}},
        {{"k3", "gg"}, {"k4", "ll"}}, {{"k3", "hh"}, {"k4", "ll"}}, {{"k3", "ii"}, {"k4", "ll"}},
    };

    std::vector<dasi::api::Key> seq3 {
        {{"k5", "mm"}, {"k6", "pp"}}, {{"k5", "nn"}, {"k6", "pp"}}, {{"k5", "oo"}, {"k6", "pp"}},
        {{"k5", "mm"}, {"k6", "qq"}}, {{"k5", "nn"}, {"k6", "qq"}}, {{"k5", "oo"}, {"k6", "qq"}},
        {{"k5", "mm"}, {"k6", "rr"}}, {{"k5", "nn"}, {"k6", "rr"}}, {{"k5", "oo"}, {"k6", "rr"}},
    };

    dasi::core::SchemaKeyIterator<decltype(v1)> ki;

    ki.insert(0, "k1", v1);
    ki.insert(0, "k2", v2);
    ki.insert(1, "k3", v3);
    ki.insert(1, "k4", v4);
    ki.insert(2, "k5", v5);
    ki.insert(2, "k6", v6);

    struct Visitor {
        int level1 = 0;
        int level2 = 0;
        int level3 = -1;
        int count = 0;
        const std::vector<dasi::api::Key>& seq1_;
        const std::vector<dasi::api::Key>& seq2_;
        const std::vector<dasi::api::Key>& seq3_;
        Visitor(const std::vector<dasi::api::Key>& seq1,
                const std::vector<dasi::api::Key>& seq2,
                const std::vector<dasi::api::Key>& seq3) :
            seq1_(seq1), seq2_(seq2), seq3_(seq3) {}
        void thirdLevel(const dasi::core::SplitReferenceKey& key) {
            count++;
            level3++;
            if (level3 >= seq3_.size()) {
                level3 = 0;
                level2++;
                if (level2 >= seq2_.size()) {
                    level2 = 0;
                    level1++;
                }
            }
            EXPECT(key[0] == seq1_[level1]);
            EXPECT(key[1] == seq2_[level2]);
            EXPECT(key[2] == seq3_[level3]);
        }
    };

    Visitor v{seq1, seq2, seq3};
    ki.visit(v);
    EXPECT(v.count == (seq1.size() * seq2.size() * seq3.size()));
}


int main(int argc, char** argv) {
    return ::dasi::util::run_tests();
}