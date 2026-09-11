#include "test/byeolTest.hpp"
#include <cstdio>
#include <fstream>
#include <sstream>

using namespace std;
using namespace by;

struct stelaWriterTest: public byeolTest {};

// §7.4 (1) canonical fixed point:
//   parse(write(T)) must succeed, and write(parse(write(T))) must equal write(T).
TEST_F(stelaWriterTest, canonicalFixedPoint) {
    const std::string script = R"SRC(
def database
    def connection
        host := "localhost"
        port := 5432
    def credentials
        user := "admin"
        password := "secret"
author := "kniz"
ver := 2.1.0
    )SRC";

    tstr<stela> root1 = stelaParser().parse(script);
    ASSERT_TRUE(root1);

    // container stela::asStr() is empty by contract; serialization is stelaWriter's job.
    ASSERT_TRUE(root1->asStr().empty());

    std::string write1 = stelaWriter().write(*root1);
    ASSERT_FALSE(write1.empty());

    tstr<stela> root2 = stelaParser().parse(write1);
    ASSERT_TRUE(root2);

    std::string write2 = stelaWriter().write(*root2);
    ASSERT_EQ(write1, write2);
}

/*
// §7.4 (2) scalar round-trip for each primitive type:
//   after parse -> set -> write -> parse, values are equal.
TEST_F(stelaWriterTest, scalarRoundTripAllTypes) {
    const std::string script = R"SRC(
stub := 0
    )SRC";

    tstr<stela> root = stelaParser().parse(script);
    ASSERT_TRUE(root);

    root->set("flag", true);
    root->set("count", (nint) 42);
    root->set("ratio", (nflt) 1.5f);
    root->set("greeting", std::string("hello"));
    root->setVersion("api", 3, 2, 1);

    std::string writeted = stelaWriter().write(*root);

    tstr<stela> reparsed = stelaParser().parse(writeted);
    ASSERT_TRUE(reparsed);

    ASSERT_TRUE(reparsed->sub("flag").asBool());
    ASSERT_EQ(reparsed->sub("count").asInt(), 42);
    ASSERT_STREQ(reparsed->sub("greeting").asStr().c_str(), "hello");

    verStela* api = reparsed->sub("api").cast<verStela>();
    ASSERT_TRUE(api != nullptr);
    ASSERT_EQ(api->asMajor(), 3);
    ASSERT_EQ(api->asMinor(), 2);
    ASSERT_EQ(api->asPatch(), 1);

    // floats: std::to_string yields "1.500000"; must re-parse to something close.
    const std::string& ratioStr = reparsed->sub("ratio").asStr();
    ASSERT_FALSE(ratioStr.empty());
    ASSERT_NE(ratioStr.find('.'), std::string::npos);
}

// §7.4 (3) container edit via set/setVersion produces valid stela syntax
//   (parseable after write).
TEST_F(stelaWriterTest, containerEditByFreshSet) {
    rootStela root("root");
    root.set("name", std::string("byeol"));
    root.set("age", (nint) 10);
    root.setVersion("ver", 1, 0, 0);

    std::string writeted = stelaWriter().write(root);

    tstr<stela> parsed = stelaParser().parse(writeted);
    ASSERT_TRUE(parsed);

    ASSERT_STREQ(parsed->sub("name").asStr().c_str(), "byeol");
    ASSERT_EQ(parsed->sub("age").asInt(), 10);

    verStela* v = parsed->sub("ver").cast<verStela>();
    ASSERT_TRUE(v != nullptr);
    ASSERT_EQ(v->asMajor(), 1);
    ASSERT_EQ(v->asMinor(), 0);
    ASSERT_EQ(v->asPatch(), 0);

    // now overwrite an existing scalar with a new scalar value:
    parsed->set("age", (nint) 99);
    std::string rewriteted = stelaWriter().write(*parsed);

    tstr<stela> reparsed = stelaParser().parse(rewriteted);
    ASSERT_TRUE(reparsed);
    ASSERT_EQ(reparsed->sub("age").asInt(), 99);
}

// §7.4 (4) type-mismatch defense:
//   set(name, scalar) on a container-typed child must refuse and preserve the child.
TEST_F(stelaWriterTest, setRefusedOnContainerChild) {
    const std::string script = R"SRC(
def group
    member := "alice"
    )SRC";

    tstr<stela> root = stelaParser().parse(script);
    ASSERT_TRUE(root);

    ncnt lenBefore = root->sub("group").len();
    ASSERT_EQ(lenBefore, 1);

    // "group" is a container. set() must refuse to overwrite it.
    root->set("group", (nint) 999);

    stela& groupAfter = root->sub("group");
    ASSERT_TRUE(groupAfter.isExist());

    // container is intact; the scalar attempt did not replace it.
    ASSERT_FALSE(groupAfter.cast<valStela>() != nullptr);
    ASSERT_EQ(groupAfter.len(), 1);
    ASSERT_STREQ(groupAfter["member"].asStr().c_str(), "alice");
}
*/
// §7.4 (5) file write E2E: write to file, re-parse from file, tree equality.
TEST_F(stelaWriterTest, writeToFileEndToEnd) {
    const std::string script = R"SRC(
def app
    name := "stela"
    port := 8080
ver := 1.2.3
    )SRC";

    tstr<stela> root = stelaParser().parse(script);
    ASSERT_TRUE(root);

    std::string path = ::testing::TempDir() + "stela_write_test.stela";
    // ensure a clean start.
    std::remove(path.c_str());

    nbool ok = stelaWriter().writeFile(*root, path);
    ASSERT_TRUE(ok);

    tstr<stela> reparsed = stelaParser().parseFromFile(path);
    ASSERT_TRUE(reparsed);

    stela& app = reparsed->sub("app");
    ASSERT_TRUE(app.isExist());

    ASSERT_STREQ(app["name"].asStr().c_str(), "stela");
    ASSERT_EQ(app["port"].asInt(), 8080);

    verStela* v = reparsed->sub("ver").cast<verStela>();
    ASSERT_TRUE(v);

    ASSERT_EQ(v->asMajor(), 1);
    ASSERT_EQ(v->asMinor(), 2);
    ASSERT_EQ(v->asPatch(), 3);

    std::remove(path.c_str());
}

TEST_F(stelaWriterTest, prefixComment) {
    const std::string script =
        "# byeol core bundle manifest.\n"
        "# generated by CI - do not edit by hand.\n"
        "requiredVer := 0.1.7\n";

    tstr<stela> root = stelaParser().parse(script);
    ASSERT_TRUE(root);
    ASSERT_EQ(stelaWriter().write(*root), script);
}

TEST_F(stelaWriterTest, postfixComment) {
    const std::string script = "size := 18240004  # bytes\n";

    tstr<stela> root = stelaParser().parse(script);
    ASSERT_TRUE(root);
    ASSERT_EQ(stelaWriter().write(*root), script);
}

TEST_F(stelaWriterTest, defKeepsBothComments) {
    const std::string script =
        "# 0.1.4.1 부터 지원.\n"
        "def latest  # 채널별 최신\n"
        "    stable := 0.1.4.1\n";

    tstr<stela> root = stelaParser().parse(script);
    ASSERT_TRUE(root);

    stela& latest = root->sub("latest");
    ASSERT_TRUE(latest.isExist());
    ASSERT_STREQ(latest.getPrefix().c_str(), "# 0.1.4.1 부터 지원.");
    ASSERT_STREQ(latest.getPostfix().c_str(), "# 채널별 최신");
    ASSERT_EQ(stelaWriter().write(*root), script);
}

TEST_F(stelaWriterTest, blockCommentKeptVerbatim) {
    const std::string script =
        "## 여러 줄\n"
        "   블록 주석 ##\n"
        "port := 5432\n";

    tstr<stela> root = stelaParser().parse(script);
    ASSERT_TRUE(root);
    ASSERT_EQ(stelaWriter().write(*root), script);
}

TEST_F(stelaWriterTest, inlineBlockCommentInArray) {
    // a comment between two elements is the prefix of the one after it, not the postfix of the one before.
    const std::string script = "deps := {1, ## c ## 3, 4}\n";

    tstr<stela> root = stelaParser().parse(script);
    ASSERT_TRUE(root);
    ASSERT_EQ(stelaWriter().write(*root), script);
}

TEST_F(stelaWriterTest, commentSurvivesValueEdit) {
    // the point of holding comments on the node: editing a value must not wipe them.
    tstr<stela> root = stelaParser().parse("size := 1  # bytes\n");
    ASSERT_TRUE(root);

    stela& size = root->sub("size");
    ASSERT_TRUE(size.isExist());

    numStela* edited = new numStela(2, "size");
    edited->setPrefix(size.getPrefix());
    edited->setPostfix(size.getPostfix());
    root->add(edited);

    ASSERT_EQ(stelaWriter().write(*root), "size := 2  # bytes\n");
}

TEST_F(stelaWriterTest, addDropsTheReplacedComment) {
    // add() puts back exactly what it was handed. losing the comment is the contract.
    tstr<stela> root = stelaParser().parse("size := 1  # bytes\n");
    ASSERT_TRUE(root);

    root->add(new numStela(2, "size"));
    ASSERT_EQ(stelaWriter().write(*root), "size := 2\n");
}

TEST_F(stelaWriterTest, commentAtEndOfFile) {
    const std::string script =
        "port := 5432\n"
        "# 여기서 끝.\n";

    tstr<stela> root = stelaParser().parse(script);
    ASSERT_TRUE(root);
    ASSERT_EQ(stelaWriter().write(*root), script);
}

TEST_F(stelaWriterTest, commentedFixedPoint) {
    const std::string script =
        "# 헤더\n"
        "def app  # 꼬리\n"
        "    name := \"stela\"  # 이름\n"
        "    deps := {1, ## c ## 3}\n";

    tstr<stela> root1 = stelaParser().parse(script);
    ASSERT_TRUE(root1);

    std::string write1 = stelaWriter().write(*root1);
    tstr<stela> root2 = stelaParser().parse(write1);
    ASSERT_TRUE(root2);

    ASSERT_EQ(write1, stelaWriter().write(*root2));
}

TEST_F(stelaWriterTest, hexKeepsItsSpelling) {
    const std::string script = "mask := 0xFF\n";

    tstr<stela> root = stelaParser().parse(script);
    ASSERT_TRUE(root);
    ASSERT_EQ(root->sub("mask").asInt(), 255);
    ASSERT_EQ(stelaWriter().write(*root), script);
}

TEST_F(stelaWriterTest, floatKeepsItsSpelling) {
    // std::to_string() used to write `1.5` back as `1.500000`.
    const std::string script = "ratio := 1.5\n";

    tstr<stela> root = stelaParser().parse(script);
    ASSERT_TRUE(root);
    ASSERT_EQ(stelaWriter().write(*root), script);
}

TEST_F(stelaWriterTest, spellingInsideArray) {
    const std::string script = "masks := {0x0F, 16, 0.5}\n";

    tstr<stela> root = stelaParser().parse(script);
    ASSERT_TRUE(root);
    ASSERT_EQ(stelaWriter().write(*root), script);
}

TEST_F(stelaWriterTest, editedNumberDropsTheOldSpelling) {
    tstr<stela> root = stelaParser().parse("mask := 0xFF\n");
    ASSERT_TRUE(root);

    root->add(new numStela(16, "mask"));
    ASSERT_EQ(stelaWriter().write(*root), "mask := 16\n");
}

TEST_F(stelaWriterTest, prefixCommentOnEveryValueKind) {
    // str and bool used to skip the pending comment, and it landed on the next number.
    const std::string script =
        "# s\n"
        "name := \"x\"\n"
        "# b\n"
        "flag := true\n"
        "port := 1\n";

    tstr<stela> root = stelaParser().parse(script);
    ASSERT_TRUE(root);
    ASSERT_EQ(stelaWriter().write(*root), script);
}

TEST_F(stelaWriterTest, prefixCommentAboveArray) {
    // the first element is created before the array, and it used to take the comment.
    const std::string script =
        "# deps\n"
        "deps := {1, 2}\n";

    tstr<stela> root = stelaParser().parse(script);
    ASSERT_TRUE(root);
    ASSERT_EQ(stelaWriter().write(*root), script);
}

TEST_F(stelaWriterTest, lineCommentInsideArray) {
    // a comment before a swallowed newline used to escape the array.
    const std::string script = "deps := {1, # c\n3}\n";

    tstr<stela> root = stelaParser().parse(script);
    ASSERT_TRUE(root);
    ASSERT_EQ(stelaWriter().write(*root), script);
}

TEST_F(stelaWriterTest, blockCommentBeforeClosingBrace) {
    // this used to escape the array and prefix the next statement.
    const std::string script =
        "deps := {1, 2 ## c ##}\n"
        "port := 1\n";

    tstr<stela> root = stelaParser().parse(script);
    ASSERT_TRUE(root);
    ASSERT_EQ(stelaWriter().write(*root), script);
}

TEST_F(stelaWriterTest, nestedMultiLineBlockComment) {
    // later lines kept their absolute indentation, so every write pushed them 4 further.
    const std::string script =
        "def a\n"
        "    ## x\n"
        "       y ##\n"
        "    k := 1\n";

    tstr<stela> root1 = stelaParser().parse(script);
    ASSERT_TRUE(root1);
    std::string write1 = stelaWriter().write(*root1);
    ASSERT_EQ(write1, script);

    tstr<stela> root2 = stelaParser().parse(write1);
    ASSERT_TRUE(root2);
    ASSERT_EQ(stelaWriter().write(*root2), write1);
}
