#include <pstd/trie/autocomplete_trie.hpp>
#include <pstd/trie/trie.hpp>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <print>
#include <random>
#include <vector>

void benchmark_autocomplete(const std::vector<std::string>& words, std::string_view prefix)
{
    pstd::trie              standard_trie;
    pstd::autocomplete_trie optimized_trie;

    for (const auto& word : words) {
        standard_trie.insert(word);
        optimized_trie.insert(word);
    }

    size_t trie_result_count = 0, optimized_trie_result_count = 0, vector_result_count = 0;

    // Standard Trie autocomplete
    auto start = std::chrono::high_resolution_clock::now();
    auto trie_results = standard_trie.get_strings_with_prefix<std::vector<std::string>>(prefix);
    auto trie_query_time = std::chrono::high_resolution_clock::now() - start;
    trie_result_count = trie_results.size();

    // Optimized Autocomplete Trie autocomplete
    start = std::chrono::high_resolution_clock::now();
    auto optimized_trie_results = optimized_trie.get_strings_with_prefix<std::vector<std::string>>(prefix);
    auto optimized_trie_query_time = std::chrono::high_resolution_clock::now() - start;
    optimized_trie_result_count = optimized_trie_results.size();

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

    auto print_time = [](auto duration, const char* action, size_t matches) {
        std::println("{}: {}μs ({} matches)",
                     action,
                     std::chrono::duration_cast<std::chrono::microseconds>(duration).count(),
                     matches);
    };

    std::println("\n=== Autocomplete Benchmark ({} words) with prefix: \"{}\" ===", words.size(), prefix);
    print_time(trie_query_time, "Standard Trie query", trie_result_count);
    print_time(optimized_trie_query_time, "Optimized Trie query", optimized_trie_result_count);
    print_time(vector_query_time, "Vector query", vector_result_count);

    if (vector_result_count > 0 && trie_result_count > 0) {
        std::println(
            "Standard Trie speedup: {:.2f}x faster than vector",
            static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(vector_query_time).count()) /
                std::chrono::duration_cast<std::chrono::microseconds>(trie_query_time).count());

        std::println(
            "Optimized Trie speedup: {:.2f}x faster than vector",
            static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(vector_query_time).count()) /
                std::chrono::duration_cast<std::chrono::microseconds>(optimized_trie_query_time).count());

        std::println(
            "Optimized vs Standard Trie: {:.2f}x faster",
            static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(trie_query_time).count()) /
                std::chrono::duration_cast<std::chrono::microseconds>(optimized_trie_query_time).count());
    }
}

void benchmark_top_suggestions(const std::vector<std::string>& words, std::string_view prefix, size_t max_count)
{
    pstd::autocomplete_trie optimized_trie;

    for (const auto& word : words) {
        optimized_trie.insert(word);
    }

    // Optimized Autocomplete Trie top suggestions
    auto start = std::chrono::high_resolution_clock::now();
    auto top_suggestions = optimized_trie.get_top_suggestions<std::vector<std::string>>(prefix, max_count);
    auto optimized_top_time = std::chrono::high_resolution_clock::now() - start;

    // Vector-based top suggestions (sort by frequency)
    start = std::chrono::high_resolution_clock::now();
    std::vector<std::string> vector_results;
    for (const auto& word : words) {
        if (word.starts_with(prefix)) {
            vector_results.push_back(word);
        }
    }

    std::sort(vector_results.begin(), vector_results.end());
    std::vector<std::pair<std::string, int>> freq_count;
    for (const auto& word : vector_results) {
        if (freq_count.empty() || freq_count.back().first != word) {
            freq_count.emplace_back(word, 1);
        } else {
            freq_count.back().second++;
        }
    }

    std::sort(freq_count.begin(), freq_count.end(), [](const auto& a, const auto& b) { return a.second > b.second; });

    // Take top N
    std::vector<std::string> vector_top_suggestions;
    for (size_t i = 0; i < std::min(max_count, freq_count.size()); ++i) {
        vector_top_suggestions.push_back(freq_count[i].first);
    }

    auto vector_top_time = std::chrono::high_resolution_clock::now() - start;

    auto print_time = [](auto duration, const char* action, size_t matches) {
        std::println("{}: {}μs ({} results)",
                     action,
                     std::chrono::duration_cast<std::chrono::microseconds>(duration).count(),
                     matches);
    };

    std::println(
        "\n=== Top Suggestions Benchmark ({} words) with prefix: \"{}\" (top {}) ===", words.size(), prefix, max_count);
    print_time(optimized_top_time, "Optimized Trie top suggestions", top_suggestions.size());
    print_time(vector_top_time, "Vector top suggestions", vector_top_suggestions.size());

    if (!vector_top_suggestions.empty()) {
        std::println(
            "Optimized Trie speedup: {:.2f}x faster than vector",
            static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(vector_top_time).count()) /
                std::chrono::duration_cast<std::chrono::microseconds>(optimized_top_time).count());
    }
}

int main()
{
    std::ifstream dict("src/big_dic.txt");
    if (dict) {
        std::vector<std::string> real_words;
        std::string              word;
        while (std::getline(dict, word)) {
            if (!word.empty() && std::all_of(word.cbegin(), word.cend(), [](char c) { return std::isalpha(c); })) {
                // Convert to lowercase for our tries
                std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
                real_words.push_back(word);
            }
        }

        benchmark_autocomplete(real_words, "acco");
        benchmark_autocomplete(real_words, "circum");
        benchmark_autocomplete(real_words, "pre");  // More common prefix
        benchmark_autocomplete(real_words, "z");    // Less common prefix

        benchmark_top_suggestions(real_words, "acc", 10);
        benchmark_top_suggestions(real_words, "comp", 5);
        benchmark_top_suggestions(real_words, "pre", 20);
    } else {
        std::println("Could not open dictionary file src/dict.txt");
    }
}
