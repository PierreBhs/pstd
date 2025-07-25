#include "autocomplete_trie.hpp"

#include <algorithm>
#include <queue>
#include <stack>

namespace pstd {

auto autocomplete_trie::insert(std::string_view word) -> void
{
    auto* current = &m_root;

    for (const auto c : word) {
        const auto [idx, valid] = char_to_index(c);
        if (!valid) [[unlikely]] {
            return;
        }

        if (!current->has_child(idx)) [[unlikely]] {
            auto* new_node = m_pool.allocate();
            current->children[idx] = new_node;
            current->set_child(idx);
        }
        current = current->children[idx];
        current->word_count++;
    }

    current->is_terminal = true;
}

auto autocomplete_trie::remove(std::string_view word) -> bool
{
    auto* current = &m_root;

    for (const auto c : word) {
        const auto [idx, valid] = char_to_index(c);
        if (!valid || !current->has_child(idx)) {
            return false;
        }
        current = current->children[idx];
    }

    if (current->is_terminal) {
        current->is_terminal = false;
        return true;
    }
    return false;
}

auto autocomplete_trie::search(std::string_view word) const -> bool
{
    const auto* current = &m_root;

    for (const auto c : word) {
        const auto [idx, valid] = char_to_index(c);
        if (!valid || !current->has_child(idx)) [[unlikely]] {
            return false;
        }
        current = current->children[idx];
    }

    return current->is_terminal;
}

auto autocomplete_trie::starts_with(std::string_view prefix) const -> bool
{
    const auto* current = &m_root;

    for (const auto c : prefix) {
        const auto [idx, valid] = char_to_index(c);
        if (!valid || !current->has_child(idx)) [[unlikely]] {
            return false;
        }
        current = current->children[idx];
    }

    return true;
}

template <StringContainerv2 Container>
auto autocomplete_trie::get_all_strings() const -> Container
{
    Container   container;
    std::string prefix;
    prefix.reserve(32);
    collect_helper(&m_root, prefix, std::inserter(container, container.end()));
    return container;
}

template <StringContainerv2 Container>
auto autocomplete_trie::get_strings_with_prefix(std::string_view prefix) const -> Container
{
    Container   container;
    const auto* node = &m_root;

    for (const auto c : prefix) {
        const auto [idx, valid] = char_to_index(c);
        if (!valid || !node->has_child(idx)) [[unlikely]] {
            return container;
        }
        node = node->children[idx];
    }

    std::string mutable_prefix{prefix};
    mutable_prefix.reserve(mutable_prefix.size() + 32);
    collect_helper(node, mutable_prefix, std::inserter(container, container.end()));
    return container;
}

auto autocomplete_trie::count_prefix_matches(std::string_view prefix) const -> size_t
{
    const auto* current = &m_root;

    for (const auto c : prefix) {
        const auto [idx, valid] = char_to_index(c);
        if (!valid || !current->has_child(idx)) [[unlikely]] {
            return 0;
        }
        current = current->children[idx];
    }

    return current->word_count;
}

template <StringContainerv2 Container>
auto autocomplete_trie::get_top_suggestions(std::string_view prefix, size_t max_count) const -> Container
{
    Container container;
    if (max_count == 0)
        return container;

    const auto* node = &m_root;

    for (const auto c : prefix) {
        const auto [idx, valid] = char_to_index(c);
        if (!valid || !node->has_child(idx)) [[unlikely]] {
            return container;
        }
        node = node->children[idx];
    }

    if (max_count <= 16) {
        struct weighted_string
        {
            std::string str;
            uint32_t    weight;

            bool operator<(const weighted_string& other) const noexcept { return weight > other.weight; }
        };

        std::priority_queue<weighted_string> pq;

        // DFS collection with weight tracking
        struct stack_item_t
        {
            const node_t* node;
            std::string   current;
        };

        std::stack<stack_item_t> stack;
        stack.emplace(node, std::string{prefix});

        while (!stack.empty() && pq.size() <= max_count * 2) {
            auto [currentNode, currentStr] = stack.top();
            stack.pop();

            if (currentNode->is_terminal) {
                if (pq.size() < max_count) {
                    pq.emplace(weighted_string{currentStr, currentNode->word_count});
                } else if (currentNode->word_count > pq.top().weight) {
                    pq.pop();
                    pq.emplace(weighted_string{currentStr, currentNode->word_count});
                }
            }

            for (size_t i = 0; i < node_t::ALPHABET_SIZE; ++i) {
                if (currentNode->has_child(i)) {
                    char c = 'a' + i;
                    stack.emplace(currentNode->children[i], currentStr + c);
                }
            }
        }

        std::vector<weighted_string> temp;
        while (!pq.empty()) {
            temp.push_back(pq.top());
            pq.pop();
        }

        // Reverse to get descending order
        for (auto it = temp.rbegin(); it != temp.rend(); ++it) {
            container.insert(container.end(), std::move(it->str));
        }
    } else {
        // For larger counts, use standard collection
        std::string mutable_prefix{prefix};
        mutable_prefix.reserve(mutable_prefix.size() + 32);
        collect_helper(node, mutable_prefix, std::inserter(container, container.end()));

        if (container.size() > max_count) {}
    }

    return container;
}

/*
 * Private Helpers
*/

template <typename OutputIt>
void autocomplete_trie::collect_helper(const node_t* node, std::string& prefix, OutputIt output) const
{
    struct stack_item_t
    {
        const node_t* node;
        size_t        prefix_length;
        char          next_char;
    };

    std::stack<stack_item_t> stack;
    stack.emplace(node, prefix.length(), '\0');

    while (!stack.empty()) {
        auto [currentNode, prefix_length, next_char] = stack.top();
        stack.pop();

        // Update prefix if needed
        if (next_char != '\0') {
            if (prefix.length() > prefix_length) {
                prefix[prefix_length] = next_char;
                prefix.resize(prefix_length + 1);
            } else {
                prefix += next_char;
            }
        } else {
            prefix.resize(prefix_length);
        }

        if (currentNode->is_terminal) {
            *output = prefix;
            ++output;
        }

        for (int i = static_cast<int>(node_t::ALPHABET_SIZE) - 1; i >= 0; --i) {
            if (currentNode->has_child(i)) {
                char c = 'a' + i;
                stack.emplace(currentNode->children[i], prefix_length + 1, c);
            }
        }
    }
}

auto autocomplete_trie::count_prefix_helper(const node_t* node) const -> size_t
{
    if (!node)
        return 0;
    return node->word_count;
}

}  // namespace pstd
