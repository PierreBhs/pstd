#include <pstd/trie/trie.hpp>

#include <stack>

namespace pstd {

auto trie::insert(std::string_view word) -> void
{
    auto* current = m_root.get();

    for (const auto c : word) {
        const auto idx = c - 'a';
        if (current->children[idx] == nullptr) {
            current->children[idx] = std::make_unique<node_t>();
        }
        current = current->children[idx].get();
    }
    
    current->is_terminal = true;
}

auto trie::remove(std::string_view word) -> void
{
    remove_helper(m_root, word, 0);
}

auto trie::search(std::string_view word) -> bool
{
    auto* current = m_root.get();

    for (const auto c : word) {
        const auto idx = c - 'a';
        if (current->children[idx] == nullptr) {
            return false;
        }
        current = current->children[idx].get();
    }
    
    return current->is_terminal;
}

auto trie::starts_with(std::string_view prefix) -> bool
{
    auto* current = m_root.get();
    
    for (auto c : prefix) {
        const auto idx = c - 'a';
        if (!current->children[idx]) {
            return false;
        }
        current = current->children[idx].get();
    }
    
    return true;
}

template <StringContainer Container>
Container trie::get_all_strings() const
{
    Container container;
    collect_helper(m_root.get(), "", std::inserter(container, container.end()));
    return container;
}

template <StringContainer Container>
Container trie::get_strings_with_prefix(std::string_view prefix) const
{
    Container   container;
    const auto* node = m_root.get();

    for (const auto c : prefix) {
        auto index = c - 'a';
        if (index < 0 || index >= 26 || !node->children[index]) {
            return container;
        }
        node = node->children[index].get();
    }

    collect_helper(node, prefix, std::inserter(container, container.end()));
    return container;
}

/*
 * Private Helpers
*/

auto trie::remove_helper(std::unique_ptr<node_t>& current, std::string_view word, size_t depth) -> bool
{
    // Reached the node representing the entire word
    if (depth == word.size()) {
        if (current->is_terminal) {
            current->is_terminal = false;
            for (auto& child : current->children) {
                if (child) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    auto c = word[depth];
    auto index = c - 'a';
    if (!current->children[index]) {
        return false;
    }

    // Recurse to child node
    auto should_remove_child = remove_helper(current->children[index], word, depth + 1);

    // Delete child if safe and prune orphaned parents
    if (should_remove_child) {
        current->children[index].reset();
        if (!current->is_terminal) {
            for (auto& child : current->children) {
                if (child) {
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

template <typename OutputIt>
void trie::collect_helper(const node_t* node, std::string_view prefix, OutputIt output) const
{
    struct stack_item_t
    {
        const node_t* node;
        std::string   current;
    };

    std::stack<stack_item_t> stack;
    stack.emplace(node, std::string{prefix});

    while (!stack.empty()) {
        auto [currentNode, currentStr] = stack.top();
        stack.pop();

        if (currentNode->is_terminal) {
            *output = currentStr;
            ++output;
        }

        for (auto i = 25; i >= 0; --i) {
            if (currentNode->children[i]) {
                char c = 'a' + i;
                stack.emplace(currentNode->children[i].get(), currentStr + c);
            }
        }
    }
}

}  // namespace pstd
