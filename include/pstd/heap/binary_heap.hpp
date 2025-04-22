#pragma once

#include <functional>
#include <iterator>
#include <vector>

namespace pstd {

template <typename T, typename Container, typename Compare>
class binary_heap;

template <typename T, typename Container = std::vector<T>, typename Compare = std::less<typename Container::value_type>>
class binary_heap
{
public:
    using container_type = Container;
    using value_compare = Compare;
    using value_type = typename Container::value_type;
    using size_type = typename Container::size_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;

    // --- Constructors ---
    binary_heap() = default;
    explicit binary_heap(const Compare& compare);
    binary_heap(const Compare& compare, const Container& cont);
    binary_heap(const Compare& compare, Container&& cont);

    template <std::input_iterator InputIt>
    binary_heap(InputIt first, InputIt last, const Compare& compare = Compare());

    // --- Element Access ---
    [[nodiscard]] const_reference top() const;

    // --- Capacity ---
    [[nodiscard]] bool      empty() const noexcept;
    [[nodiscard]] size_type size() const noexcept;

    // --- Modifiers ---
    void push(const value_type& value);
    void push(value_type&& value);

    template <typename... Args>
    void emplace(Args&&... args);

    void pop();

    void swap(binary_heap& other) noexcept(std::is_nothrow_swappable_v<Container> &&
                                           std::is_nothrow_swappable_v<Compare>);

private:
    Container m_c;
    Compare   m_comp;
    // --- Heap Algorithms (Manual Implementation) ---
    void sift_up(size_type index);
    void sift_down(size_type index);
    void heapify();  // Build heap from existing container

    friend void swap(binary_heap& lhs, binary_heap& rhs) noexcept(noexcept(lhs.swap(rhs))) { lhs.swap(rhs); }
};

// Deduction Guides
template <std::input_iterator InputIt,
          typename Compare = std::less<typename std::iterator_traits<InputIt>::value_type>,
          typename Container = std::vector<typename std::iterator_traits<InputIt>::value_type>>
binary_heap(InputIt, InputIt, Compare = Compare())
    -> binary_heap<typename std::iterator_traits<InputIt>::value_type, Container, Compare>;

}  // namespace pstd

#include "binary_heap.tpp"
