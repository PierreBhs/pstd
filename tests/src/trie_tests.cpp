#include <gtest/gtest.h>

#include <pstd/trie/trie.hpp>

#include <vector>
#include <deque>
#include <list>
#include <set>

class TrieTest : public ::testing::Test {
protected:
    pstd::trie trie;
    
    void SetUp() override {
        trie.insert("apple");
        trie.insert("app");
        trie.insert("banana");
        trie.insert("application");
        trie.insert("cat");
    }
};

// Insert/Search tests
TEST_F(TrieTest, InsertSearch) {
    EXPECT_TRUE(trie.search("apple"));
    EXPECT_FALSE(trie.search("ap"));
    EXPECT_TRUE(trie.starts_with("ap"));
    EXPECT_FALSE(trie.search("dog"));
}

// Remove tests
TEST_F(TrieTest, RemoveWord) {
    trie.remove("app");
    EXPECT_FALSE(trie.search("app"));
    EXPECT_TRUE(trie.search("apple"));
    EXPECT_TRUE(trie.starts_with("app"));
}

TEST_F(TrieTest, RemoveNonexistent) {
    trie.remove("xyz");
    EXPECT_TRUE(trie.search("cat"));
}

// Container return tests
TEST_F(TrieTest, GetAllStringsVector) {
    auto words = trie.get_all_strings<std::vector<std::string>>();
    std::set<std::string> wordSet(words.begin(), words.end());
    EXPECT_EQ(wordSet, std::set<std::string>({"app", "apple", "application", "banana", "cat"}));
}

TEST_F(TrieTest, GetAllStringsDeque) {
    auto words = trie.get_all_strings<std::deque<std::string>>();
    EXPECT_EQ(words.size(), 5);
    EXPECT_NE(std::find(words.begin(), words.end(), "banana"), words.end());
}

TEST_F(TrieTest, GetAllStringsList) {
    auto words = trie.get_all_strings<std::list<std::string>>();
    EXPECT_EQ(std::count(words.begin(), words.end(), "app"), 1);
}

TEST_F(TrieTest, GetAllStringsSet) {
    auto words = trie.get_all_strings<std::set<std::string>>();
    EXPECT_EQ(words, std::set<std::string>({"app", "apple", "application", "banana", "cat"}));
}

TEST_F(TrieTest, GetStringsWithPrefixVector) {
    auto words = trie.get_strings_with_prefix<std::vector<std::string>>("app");
    std::set<std::string> wordSet(words.begin(), words.end());
    EXPECT_EQ(wordSet, std::set<std::string>({"app", "apple", "application"}));
}

TEST_F(TrieTest, GetStringsWithPrefixDeque) {
    auto words = trie.get_strings_with_prefix<std::deque<std::string>>("b");
    EXPECT_EQ(words.size(), 1);
    EXPECT_EQ(words[0], "banana");
}

TEST_F(TrieTest, GetStringsWithPrefixNone) {
    auto words = trie.get_strings_with_prefix<std::vector<std::string>>("xyz");
    EXPECT_TRUE(words.empty());
}

// Edge cases
TEST(TrieEdgeCases, EmptyTrie) {
    pstd::trie empty;
    EXPECT_FALSE(empty.search("test"));
    auto words = empty.get_all_strings<std::vector<std::string>>();
    EXPECT_TRUE(words.empty());
}

TEST(TrieEdgeCases, EmptyString) {
    pstd::trie trie;
    trie.insert("");
    EXPECT_TRUE(trie.search(""));
    auto words = trie.get_all_strings<std::set<std::string>>();
    EXPECT_EQ(words, std::set<std::string>({""}));
}
