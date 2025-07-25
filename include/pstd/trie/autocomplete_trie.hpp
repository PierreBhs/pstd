#pragma once

#include <array>
#include <bit>
#include <concepts>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

/*
    Trie implementation optimized for autocompletion tasks,
*/

namespace pstd {

/* Concept needed for get_all_strings and get_strings_with_prefix */

template <typename Container>
concept StringContainerv2 = requires(Container c, std::string s) {
    requires std::default_initializable<Container>;
    requires std::same_as<typename Container::value_type, std::string>;
    // Must support insertion at end
    { c.insert(c.end(), s) } -> std::same_as<typename Container::iterator>;
};

class autocomplete_trie
{
public:
    /* Modifiers */

    auto insert(std::string_view word) -> void;
    auto remove(std::string_view word) -> bool;

    /* Element access */

    auto search(std::string_view word) const -> bool;
    auto starts_with(std::string_view prefix) const -> bool;

    template <StringContainerv2 Container>
    auto get_all_strings() const -> Container;

    template <StringContainerv2 Container>
    auto get_strings_with_prefix(std::string_view prefix) const -> Container;

    /* Autocomplete specific methods */

    auto count_prefix_matches(std::string_view prefix) const -> size_t;

    template <StringContainerv2 Container>
    auto get_top_suggestions(std::string_view prefix, size_t max_count) const -> Container;

private:
    struct alignas(64) node_t
    {
        static constexpr size_t ALPHABET_SIZE = 26;

        uint32_t child_mask{0};
        bool     is_terminal{false};
        uint32_t word_count{1};

        std::array<node_t*, ALPHABET_SIZE> children{};

        node_t() = default;

        [[nodiscard]] auto has_child(size_t idx) const noexcept -> bool { return (child_mask & (1U << idx)) != 0; }
        auto               set_child(size_t idx) noexcept -> void { child_mask |= (1U << idx); }
        auto               clear_child(size_t idx) noexcept -> void { child_mask &= ~(1U << idx); }
        [[nodiscard]] auto child_count() const noexcept -> size_t { return std::popcount(child_mask); }
    };

    class node_pool
    {
    private:
        static constexpr size_t POOL_SIZE = 4096;  // Tune based on usage
        struct pool_block
        {
            alignas(64) std::array<node_t, POOL_SIZE> nodes;
            size_t                      used = 0;
            std::unique_ptr<pool_block> next;
        };

        std::unique_ptr<pool_block> m_current_pool{std::make_unique<pool_block>()};
        pool_block*                 m_current_pool_ptr{m_current_pool.get()};

    public:
        auto allocate() -> node_t*
        {
            if (m_current_pool_ptr->used >= POOL_SIZE) {
                auto new_block = std::make_unique<pool_block>();
                m_current_pool_ptr->next = std::move(new_block);
                m_current_pool_ptr = m_current_pool_ptr->next.get();
                m_current_pool_ptr->used = 0;
            }

            return &m_current_pool_ptr->nodes[m_current_pool_ptr->used++];
        }

        auto reset() noexcept -> void
        {
            m_current_pool_ptr = m_current_pool.get();
            m_current_pool_ptr->used = 0;
            m_current_pool_ptr->next.reset();
        }
    };

    auto remove_helper(node_t* current, std::string_view word, size_t depth) -> bool;

    template <typename OutputIt>
    void collect_helper(const node_t* node, std::string& prefix, OutputIt output) const;

    auto                      count_prefix_helper(const node_t* node) const -> size_t;
    [[nodiscard]] static auto char_to_index(char c) noexcept -> std::pair<size_t, bool>
    {
        const auto idx = static_cast<size_t>(c - 'a');
        return {idx, (idx < 26)};
    }

    node_t    m_root{};
    node_pool m_pool{};
};

}  // namespace pstd

#include "autocomplete_trie.tpp"
