#include "test/byeolTest.hpp"

using namespace std;
using namespace by;

struct rootStelaTest: public byeolTest {};

TEST_F(rootStelaTest, parserProducesExactlyOneRoot) {
    tstr<stela> root = stelaParser().parse(R"SRC(
def outer
    def inner
        name := "byeol"
    )SRC");
    ASSERT_TRUE(root);

    ASSERT_TRUE(root->cast<rootStela>() != nullptr);
    ASSERT_STREQ(root->getName().c_str(), "root");

    // nesting does not make more roots: every block below is a plain defStela.
    stela& outer = root->sub("outer");
    ASSERT_TRUE(outer.cast<defStela>() != nullptr);
    ASSERT_TRUE(outer.cast<rootStela>() == nullptr);
    ASSERT_TRUE(outer["inner"].cast<rootStela>() == nullptr);
}

TEST_F(rootStelaTest, rebuildingTheRootKeepsItsChildren) {
    // the parser hands its children from the grammar's defStela to a fresh rootStela.
    // if that transfer dropped a strong reference the values would be gone or freed.
    tstr<stela> root = stelaParser().parse(R"SRC(
name := "byeol"
port := 8080
def group
    member := "alice"
    )SRC");
    ASSERT_TRUE(root);

    ASSERT_EQ(root->len(), (ncnt) 3);
    ASSERT_STREQ((*root)["name"].asStr().c_str(), "byeol");
    ASSERT_EQ((*root)["port"].asInt(), 8080);
    ASSERT_STREQ(root->sub("group")["member"].asStr().c_str(), "alice");
}

TEST_F(rootStelaTest, rootIsNotWrittenBackOut) {
    const string script = "name := \"byeol\"\n";

    tstr<stela> root = stelaParser().parse(script);
    ASSERT_TRUE(root);

    // no `def root` line, and the top-level child stays at column 0.
    ASSERT_EQ(stelaWriter().write(*root), script);
}

TEST_F(rootStelaTest, aNestedBlockNamedRootIsStillWritten) {
    // the old depth test would have skipped this block had it been visited alone.
    // rootness is the type now, so the name is irrelevant.
    rootStela unit("root");
    defStela* inner = new defStela("root");
    inner->add(new strStela("byeol", "name"));
    unit.add(inner);

    ASSERT_EQ(stelaWriter().write(unit), "def root\n    name := \"byeol\"\n");
}

TEST_F(rootStelaTest, writerRejectsANonRootStela) {
    defStela bare("group");
    bare.add(new strStela("alice", "member"));

    ASSERT_STREQ(stelaWriter().write(bare).c_str(), "");
}
