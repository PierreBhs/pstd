#pragma once

#include <array>
#include <memory>

namespace pstd {

/* Concept needed form get_all_strings and get_strings_with_prefix */

template <typename Container>
concept StringContainer = requires(Container c, std::string s) {
    requires std::default_initializable<Container>;
    requires std::same_as<typename Container::value_type, std::string>;

    // Must support insertion at end
    { c.insert(c.end(), s) } -> std::same_as<typename Container::iterator>;
};

class trie
{
public:
    /* Modifiers */

    auto insert(std::string_view) -> void;
    auto remove(std::string_view) -> void;
    auto search(std::string_view) -> bool;
    auto starts_with(std::string_view) -> bool;

    /* Element access */

    template <StringContainer Container>
    Container get_all_strings() const;

    template <StringContainer Container>
    Container get_strings_with_prefix(std::string_view prefix) const;

private:
    struct node_t
    {
        std::array<std::unique_ptr<node_t>, 26> children{};
        bool                                    is_terminal{false};
    };

    auto removeHelper(std::unique_ptr<node_t>& current, std::string_view word, size_t depth) -> bool;
    template <typename OutputIt>
    void collectHelper(const node_t* node, std::string_view prefix, OutputIt output) const;

    std::unique_ptr<node_t> m_root{std::make_unique<node_t>()};
};

}  // namespace pstd

#include "trie.tpp"
