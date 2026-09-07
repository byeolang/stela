#include "test/byeolTest.hpp"

using namespace std;
using namespace by;

struct verStelaRevTest: public byeolTest {};

TEST_F(verStelaRevTest, readsTheFourthSegment) {
    verStela ver("0.1.4.1");
    ASSERT_STREQ(ver.asStr().c_str(), "0.1.4.1");
    ASSERT_EQ(ver.asMajor(), 0);
    ASSERT_EQ(ver.asMinor(), 1);
    ASSERT_EQ(ver.asPatch(), 4);
    ASSERT_EQ(ver.asRev(), 1);

    ASSERT_TRUE(ver == verStela(0, 1, 4, 1));
}

TEST_F(verStelaRevTest, anAbsentRevStaysAbsentButReadsAsZero) {
    // three segments is the common case; padding it to four would rewrite every
    // document that spells one out.
    verStela three("0.1.4");
    ASSERT_STREQ(three.asStr().c_str(), "0.1.4");
    ASSERT_EQ(three.asRev(), 0);

    // still padded up to three, as before.
    ASSERT_STREQ(verStela("1.2").asStr().c_str(), "1.2.0");
    ASSERT_EQ(verStela("1.2").asRev(), 0);
}

TEST_F(verStelaRevTest, missingAndExplicitZeroRevAreTheSameVersion) {
    // operator== stopped being a string compare for exactly this pair.
    ASSERT_TRUE(verStela("0.1.4") == verStela("0.1.4.0"));
    ASSERT_FALSE(verStela("0.1.4") != verStela("0.1.4.0"));
    ASSERT_FALSE(verStela("0.1.4") < verStela("0.1.4.0"));
    ASSERT_FALSE(verStela("0.1.4") > verStela("0.1.4.0"));
}

TEST_F(verStelaRevTest, revOrdersWithinOneUpstreamVersion) {
    // a re-release of the same 0.1.4 outranks the first one, and any patch move
    // outranks every rev of the version below it.
    ASSERT_TRUE(verStela("0.1.4.1") > verStela("0.1.4.0"));
    ASSERT_TRUE(verStela("0.1.5.0") > verStela("0.1.4.1"));

    // the rev is the last tiebreak, never able to outrank a patch.
    ASSERT_TRUE(verStela("0.1.5.0") > verStela("0.1.4.9"));
}

TEST_F(verStelaRevTest, parserAcceptsAFourSegmentLiteral) {
    tstr<stela> root = stelaParser().parse("ver := 0.1.4.1\n");
    ASSERT_TRUE(root);

    verStela* ver = (*root)["ver"].cast<verStela>();
    ASSERT_TRUE(ver != nullptr);
    ASSERT_EQ(ver->asRev(), 1);

    // and it survives a write/parse round trip with its fourth segment intact.
    ASSERT_EQ(stelaWriter().write(*root), "ver := 0.1.4.1\n");
}

TEST_F(verStelaRevTest, aMalformedVersionNoLongerEscapesTheParser) {
    // the VERVAL pattern spelled its dots unescaped, so `1x2y3` lexed as a version
    // and verStela's ctor threw std::invalid_argument out through parse().
    ASSERT_NO_THROW(stelaParser().parse("v := 1x2y3\n"));
}
