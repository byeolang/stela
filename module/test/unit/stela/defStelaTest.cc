#include "test/byeolTest.hpp"
#include <vector>

using namespace std;
using namespace by;

struct defStelaTest: public byeolTest {};

namespace {
    // records which onVisit overload each node lands on. the split only pays off if
    // a block and an array stop sharing one, so the test has to observe dispatch
    // itself -- the written text alone can't tell a typed overload from a fallback.
    struct dispatchProbe: public stelaVisitor {
        nbool onVisit(const stelaVisitInfo& i, defStela& it) override {
            defs.push_back(it.getName());
            return true;
        }

        nbool onVisit(const stelaVisitInfo& i, arrStela& it) override {
            arrs.push_back(it.getName());
            return true;
        }

        nbool onVisit(const stelaVisitInfo& i, stela& it) override {
            bases.push_back(it.getName());
            return true;
        }

        vector<string> defs;
        vector<string> arrs;
        vector<string> bases;
    };
}

TEST_F(defStelaTest, stelaIsAbstract) {
    // ADT withholds the clone() definition, so clonable's pure virtual survives.
    // this is what stops `new stela()` from coming back.
    ASSERT_TRUE(std::is_abstract<stela>::value);
    ASSERT_FALSE(std::is_abstract<defStela>::value);
}

TEST_F(defStelaTest, parserBuildsBlocksAsDefStela) {
    tstr<stela> root = stelaParser().parse(R"SRC(
def man
    name := "dark souls"
    )SRC");
    ASSERT_TRUE(root);

    // the implicit compilation unit is a block too, not a bare stela.
    ASSERT_TRUE(root->cast<defStela>() != nullptr);

    stela& man = root->sub("man");
    ASSERT_TRUE(man.isExist());
    ASSERT_TRUE(man.cast<defStela>() != nullptr);
}

TEST_F(defStelaTest, arrayIsNotABlock) {
    // the reason for the split: arrStela used to inherit the block form from stela.
    tstr<stela> root = stelaParser().parse("deps := {10, 20}\n");
    ASSERT_TRUE(root);

    stela& deps = root->sub("deps");
    ASSERT_TRUE(deps.cast<arrStela>() != nullptr);
    ASSERT_TRUE(deps.cast<defStela>() == nullptr);
}

TEST_F(defStelaTest, blockAndArrayDispatchToTheirOwnOverload) {
    tstr<stela> root = stelaParser().parse(R"SRC(
def group
    deps := {1, 2}
    )SRC");
    ASSERT_TRUE(root);

    dispatchProbe probe;
    probe.work(*root);

    // root and group, in visit order.
    ASSERT_EQ(probe.defs.size(), (size_t) 2);
    ASSERT_STREQ(probe.defs[1].c_str(), "group");

    ASSERT_EQ(probe.arrs.size(), (size_t) 1);
    ASSERT_STREQ(probe.arrs[0].c_str(), "deps");

    // the probe leaves valStela alone, so the two elements chain up to the base
    // overload. that path is the fallback -- before the split it also emitted
    // `def <name>`, which is exactly what a scalar must never reach.
    ASSERT_EQ(probe.bases.size(), (size_t) 2);
}

TEST_F(defStelaTest, handBuiltBlockWritesAsDefSyntax) {
    defStela root("root");
    defStela* group = new defStela("group");
    group->add(new strStela("alice", "member"));
    root.add(group);

    const string written = stelaWriter().write(root);

    tstr<stela> reparsed = stelaParser().parse(written);
    ASSERT_TRUE(reparsed);
    ASSERT_TRUE(reparsed->sub("group").cast<defStela>() != nullptr);
    ASSERT_STREQ(reparsed->sub("group")["member"].asStr().c_str(), "alice");
}
