#include "test/byeolTest.hpp"
#include <sstream>

using namespace std;
using namespace by;

struct arrStelaTest: public byeolTest {};

TEST_F(arrStelaTest, parseKeepsEveryElement) {
    tstr<stela> root = stelaParser().parse("deps := {10, 20, 30}\n");
    ASSERT_TRUE(root);

    stela& deps = root->sub("deps");
    ASSERT_TRUE(deps.isExist());
    ASSERT_TRUE(deps.cast<arrStela>() != nullptr);

    // every element survives. previously they all keyed on "" and overwrote one another.
    ASSERT_EQ(deps.len(), 3);
    ASSERT_EQ(deps.sub(0).asInt(), 10);
    ASSERT_EQ(deps.sub(1).asInt(), 20);
    ASSERT_EQ(deps.sub(2).asInt(), 30);
}

TEST_F(arrStelaTest, singleElementKeepsItsValue) {
    // `new stela{elem}` used to bind the copy ctor and slice the valStela away.
    tstr<stela> root = stelaParser().parse("only := {42}\n");
    ASSERT_TRUE(root);

    stela& only = root->sub("only");
    ASSERT_EQ(only.len(), 1);
    ASSERT_EQ(only.sub(0).asInt(), 42);
}

TEST_F(arrStelaTest, duplicatedElementsAreKept) {
    tstr<stela> root = stelaParser().parse("dup := {7, 7, 7}\n");
    ASSERT_TRUE(root);

    ASSERT_EQ(root->sub("dup").len(), 3);
}

TEST_F(arrStelaTest, indexOrderSurvivesPastTen) {
    // the regression the zero-padded names exist for: with bare "2" and "10" the
    // child map would order "10" first and sub(2) would return the wrong element.
    std::stringstream ss;
    ss << "big := {";
    for(nint n = 0; n < 12; ++n) {
        if(n) ss << ", ";
        ss << n * 100;
    }
    ss << "}\n";

    tstr<stela> root = stelaParser().parse(ss.str());
    ASSERT_TRUE(root);

    stela& big = root->sub("big");
    ASSERT_EQ(big.len(), 12);
    for(nint n = 0; n < 12; ++n)
        ASSERT_EQ(big.sub(n).asInt(), n * 100);
}

TEST_F(arrStelaTest, iteratorWalksInIndexOrder) {
    tstr<stela> root = stelaParser().parse("big := {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}\n");
    ASSERT_TRUE(root);

    nint expect = 0;
    for(auto& e: root->sub("big")) {
        stela& elem = e.get() OR_CONTINUE;
        ASSERT_EQ(elem.asInt(), expect);

        char key[16];
        snprintf(key, sizeof(key), "%04d", (int) expect);
        ASSERT_STREQ(elem.getName().c_str(), key);
        expect++;
    }
    ASSERT_EQ(expect, 12);
}

TEST_F(arrStelaTest, namedChildrenKeepInsertionOrder) {
    // these used to come back alphabetically, which reordered a file on every write and
    // made "the last entry is the newest" false.
    tstr<stela> root = stelaParser().parse("zebra := 1\napple := 2\nmango := 3\n");
    ASSERT_TRUE(root);

    std::string names;
    for(auto& e: *root)
        names += e->getName() + " ";
    ASSERT_STREQ(names.c_str(), "zebra apple mango ");
}

TEST_F(arrStelaTest, mixedTypesAndStrings) {
    tstr<stela> root = stelaParser().parse("mix := {1, \"two\", true, 3.5}\n");
    ASSERT_TRUE(root);

    stela& mix = root->sub("mix");
    ASSERT_EQ(mix.len(), 4);
    ASSERT_EQ(mix.sub(0).asInt(), 1);
    ASSERT_STREQ(mix.sub(1).asStr().c_str(), "two");
    ASSERT_TRUE(mix.sub(2).asBool());
    ASSERT_TRUE(mix.sub(3).cast<strStela>() == nullptr);
}

TEST_F(arrStelaTest, writeEmitsInlineBraces) {
    tstr<stela> root = stelaParser().parse("deps := {1, 2, 3}\n");
    ASSERT_TRUE(root);

    std::string written = stelaWriter().write(*root);
    ASSERT_STREQ(written.c_str(), "deps := {1, 2, 3}\n");
}

TEST_F(arrStelaTest, writeQuotesStringElements) {
    tstr<stela> root = stelaParser().parse("names := {\"a\", \"b\"}\n");
    ASSERT_TRUE(root);

    ASSERT_STREQ(stelaWriter().write(*root).c_str(), "names := {\"a\", \"b\"}\n");
}

TEST_F(arrStelaTest, nestedArraysStayInline) {
    tstr<stela> root = stelaParser().parse("grid := {{1, 2}, {3, 4}, {}}\n");
    ASSERT_TRUE(root);

    stela& grid = root->sub("grid");
    ASSERT_EQ(grid.len(), 3);
    ASSERT_EQ(grid.sub(0).len(), 2);
    ASSERT_EQ(grid.sub(1).sub(0).asInt(), 3);
    ASSERT_EQ(grid.sub(2).len(), 0);

    ASSERT_STREQ(stelaWriter().write(*root).c_str(), "grid := {{1, 2}, {3, 4}, {}}\n");
}

TEST_F(arrStelaTest, canonicalFixedPointWithArrays) {
    const std::string script = R"SRC(
def pkg
    name := "stela"
    deps := {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}
    tags := {"a", "b"}
    )SRC";

    tstr<stela> root1 = stelaParser().parse(script);
    ASSERT_TRUE(root1);

    std::string write1 = stelaWriter().write(*root1);
    tstr<stela> root2 = stelaParser().parse(write1);
    ASSERT_TRUE(root2);

    ASSERT_EQ(write1, stelaWriter().write(*root2));
    ASSERT_EQ(root2->sub("pkg").sub("deps").len(), 11);
    ASSERT_EQ(root2->sub("pkg").sub("deps").sub(10).asInt(), 11);
}

TEST_F(arrStelaTest, emptyArrayRoundTrips) {
    tstr<stela> root = stelaParser().parse("none := {}\n");
    ASSERT_TRUE(root);

    stela& none = root->sub("none");
    ASSERT_TRUE(none.cast<arrStela>() != nullptr);
    ASSERT_EQ(none.len(), 0);
    ASSERT_STREQ(stelaWriter().write(*root).c_str(), "none := {}\n");
}

TEST_F(arrStelaTest, missingChildDoesNotInsertPhantom) {
    // stela::sub(name) used to go through map::operator[], inserting a blank child.
    tstr<stela> root = stelaParser().parse("real := 1\n");
    ASSERT_TRUE(root);

    ASSERT_EQ(root->len(), 1);
    ASSERT_FALSE(root->sub("nope").isExist());
    ASSERT_FALSE(root->has("nope"));
    ASSERT_EQ(root->len(), 1);
}
