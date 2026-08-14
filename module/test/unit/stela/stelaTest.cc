#include "test/byeolTest.hpp"

using namespace std;
using namespace by;

struct stelaTest: public byeolTest {};

TEST_F(stelaTest, checkDefaultNulStela) {
    const stela& nul = nulStela::get();
    ASSERT_TRUE(&nul);
    ASSERT_EQ(nul.asChar(), '\0');
    ASSERT_EQ(nul.asInt(), 0);
    ASSERT_EQ(nul.asBool(), false);
    ASSERT_EQ(nul.asStr(), "");
    ASSERT_TRUE(&nul.getType());
}

TEST_F(stelaTest, checkVerStela) {
    verStela s(1, 2, 3);
    ASSERT_EQ(s.asMajor(), 1);
    ASSERT_EQ(s.asMinor(), 2);
    ASSERT_EQ(s.asFix(), 3);

    verStela s2("1.2.3");
    ASSERT_EQ(s2.asMajor(), 1);
    ASSERT_EQ(s2.asMinor(), 2);
    ASSERT_EQ(s2.asFix(), 3);

    ASSERT_TRUE(s == s2);

    verStela s3(2, 0, 0);
    ASSERT_TRUE(s < s3);
}

TEST_F(stelaTest, addOverwritesChildWithSameName) {
    // stela's strong-ref tstr only takes ownership of heap instances, so real
    // usage (mirrored by stelaParser) always adds `new`-allocated children.
    stela root("root");
    root.add(new strStela("first", "key"));
    ASSERT_EQ(root.len(), (ncnt) 1);
    ASSERT_STREQ(root["key"].asStr().c_str(), "first");

    // second add() with the same name must replace the first — not silently drop.
    root.add(new strStela("second", "key"));
    ASSERT_EQ(root.len(), (ncnt) 1);
    ASSERT_STREQ(root["key"].asStr().c_str(), "second");

    // batch overload (initializer_list) shares the same insert_or_assign path,
    // so duplicates within one batch also collapse to the last one.
    stela batch("batch");
    batch.add({new strStela("one", "k"), new strStela("two", "k")});
    ASSERT_EQ(batch.len(), (ncnt) 1);
    ASSERT_STREQ(batch["k"].asStr().c_str(), "two");
}

TEST_F(stelaTest, literalPicksStringOverload) {
    // Without a dedicated `const nchar*` ctor, a bare string literal would resolve
    // to the `nbool` overload via C++'s standard pointer-to-bool conversion (which
    // beats the user-defined `const char*` -> std::string), producing "true".
    strStela sLit("hello");
    ASSERT_STREQ(sLit.asStr().c_str(), "hello");

    // Empty literal is still a non-null pointer — must NOT collapse to `true`.
    strStela sEmpty("");
    ASSERT_STREQ(sEmpty.asStr().c_str(), "");

    // Explicit std::string still routes to the string overload — unchanged.
    strStela sStd(std::string("world"));
    ASSERT_STREQ(sStd.asStr().c_str(), "world");

    // the real `bool` path is intact — passing an actual bool still routes to (B).
    valStela vTrue(true);
    ASSERT_STREQ(vTrue.asStr().c_str(), "true");
    ASSERT_TRUE(vTrue.asBool());

    valStela vFalse(false);
    ASSERT_STREQ(vFalse.asStr().c_str(), "false");
    ASSERT_FALSE(vFalse.asBool());
}
