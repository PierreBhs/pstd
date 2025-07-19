#include <pstd/ptrs/unique_ptr.hpp>
#include <pstd/trie/trie.hpp>
#include <pstd/vector/vector.hpp>

#include <print>
#include <set>

int main()
{
    // Vector example
    pstd::vector<int> nums{1, 2, 3};
    nums.push_back(4);

    std::print("Vector contents: ");
    for (const auto& n : nums) {
        std::print("{} ", n);
    }
    std::println();

    // Pointer example
    auto ptr = pstd::make_unique<double>(3.14159);
    std::println("Unique ptr value: {}", *ptr);

    // Trie example
    pstd::trie dictionary;
    dictionary.insert("algorithm");
    dictionary.insert("algebra");
    dictionary.insert("binary");

    std::print("Trie elements in vector: ");
    for (const auto& str : dictionary.get_all_strings<pstd::vector<std::string>>()) {
        std::print("{} ", str);
    }
    std::println();

    dictionary.insert("alg");
    dictionary.remove("algebra");
    std::print("Trie elements in set: ");
    for (const auto& str : dictionary.get_strings_with_prefix<std::set<std::string>>("alg")) {
        std::print("{} ", str);
    }

    return 0;
}
