#include <pstd/trie/trie.hpp>

#include <chrono>
#include <fstream>
#include <iostream>
#include <print>
#include <vector>

void benchmark_autocomplete(const std::vector<std::string>& words, std::string_view prefix)
{
    pstd::trie trie;
    for (const auto& word : words) {
        trie.insert(word);
    }

    size_t trie_result_count = 0, vector_result_count = 0;

    // Trie autocomplete
    auto start = std::chrono::high_resolution_clock::now();
    auto trie_results = trie.get_strings_with_prefix<std::vector<std::string>>(prefix);
    auto trie_query_time = std::chrono::high_resolution_clock::now() - start;
    trie_result_count = trie_results.size();

    // Vector autocomplete (linear scan)
    std::vector<std::string> vector_db{words}, vector_results{};
    start = std::chrono::high_resolution_clock::now();
    for (const auto& word : vector_db) {
        if (word.starts_with(prefix)) {
            vector_results.push_back(word);
        }
    }
    auto vector_query_time = std::chrono::high_resolution_clock::now() - start;
    vector_result_count = vector_results.size();

    auto print_time = [](auto duration, const char* action) {
        std::println("{}: {}μs", action, std::chrono::duration_cast<std::chrono::microseconds>(duration).count());
    };

    std::println("\n=== Autocomplete Benchmark ({} words) with prefix: \"{}\" ===", words.size(), prefix);
    print_time(trie_query_time, "Trie query    ");
    print_time(vector_query_time, "Vector query  ");
    std::cout << "Trie found: " << trie_result_count << " matches\n";
    std::cout << "Vector found: " << vector_result_count << " matches\n";
    std::cout << "Speed ratio: "
              << std::chrono::duration_cast<std::chrono::microseconds>(vector_query_time).count() * 1.0 /
                     std::chrono::duration_cast<std::chrono::microseconds>(trie_query_time).count()
              << "x faster\n";
}

int main()
{
    std::ifstream dict("src/dict.txt");
    if (dict) {
        std::vector<std::string> real_words;
        std::string              word;
        while (std::getline(dict, word)) {
            if (!word.empty() && std::all_of(word.cbegin(), word.cend(), [](char c) { return std::isalpha(c); })) {
                real_words.push_back(word);
            }
        }
        benchmark_autocomplete(real_words, "acco");
        benchmark_autocomplete(real_words, "circum");
    }
}
