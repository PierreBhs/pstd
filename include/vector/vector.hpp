#pragma once

#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <limits>

namespace pstd {

template <typename T>
class vector
{
public:
    /* C++ Standard Named Requirements for Containers */
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using iterator = T*;
    using const_iterator = const T*;
    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;

    /* Constructors */

    constexpr vector() noexcept;

    explicit constexpr vector(size_type);

    constexpr vector(size_type, const_reference);

    constexpr vector(const vector&);

    template <typename InputIt>
    constexpr vector(InputIt first, InputIt last);

    constexpr vector(vector&&) noexcept = default;

    constexpr explicit vector(std::initializer_list<value_type>);

    constexpr ~vector();

    /* Element access */

    [[nodiscard]] constexpr reference       operator[](size_type);
    [[nodiscard]] constexpr const_reference operator[](size_type) const;

    [[nodiscard]] constexpr reference       front();
    [[nodiscard]] constexpr const_reference front() const;

    [[nodiscard]] constexpr reference       back();
    [[nodiscard]] constexpr const_reference back() const;

    [[nodiscard]] constexpr pointer       data() noexcept { return m_data; }
    [[nodiscard]] constexpr const_pointer data() const noexcept { return m_data; }

    /* Iterators */

    [[nodiscard]] constexpr inline iterator       begin() { return &m_data[0]; }
    [[nodiscard]] constexpr inline const_iterator begin() const { return &m_data[0]; }
    [[nodiscard]] constexpr inline const_iterator cbegin() const noexcept { return &m_data[0]; }

    [[nodiscard]] constexpr inline iterator       end() noexcept { return (m_data + size()); }
    [[nodiscard]] constexpr inline const_iterator end() const noexcept { return (m_data + size()); }
    [[nodiscard]] constexpr inline const_iterator cend() const noexcept { return (m_data + size()); }

    /* Capacity */

    [[nodiscard]] constexpr inline bool      empty() const { return begin() == end(); }
    [[nodiscard]] constexpr inline size_type size() const { return m_size; }
    [[nodiscard]] constexpr inline size_type max_size() const { return std::numeric_limits<size_type>::max(); }
    [[nodiscard]] constexpr inline size_type capacity() const { return m_capacity; }
    constexpr void                           reserve(size_type);

    /* Modifiers */

    constexpr void clear() noexcept;

    constexpr iterator insert(const_iterator pos, const_reference)
        requires(std::is_copy_constructible_v<value_type> && std::is_copy_assignable_v<value_type>);
    constexpr iterator insert(const_iterator pos, T&&)
        requires(std::is_move_constructible_v<value_type> && std::is_move_assignable_v<value_type>);

    template <class... Args>
    constexpr reference emplace_back(Args&&...)
        requires std::move_constructible<value_type>;
    // Also needs to be EmplaceConstructible, how to check ?

    constexpr void push_back(const_reference)
        requires std::copy_constructible<value_type>;
    constexpr void push_back(T&& value)
        requires std::move_constructible<value_type>;

    constexpr void pop_back();

    constexpr void resize(size_type)
        requires(std::is_move_constructible_v<value_type> && std::is_default_constructible_v<value_type>);
    constexpr void resize(size_type, const_reference)
        requires std::is_copy_constructible_v<value_type>;

private:
    constexpr pointer grow(size_type, bool);

    constexpr pointer allocate(size_type);
    constexpr void    destroy();

    constexpr void default_append(size_type);
    constexpr void append(size_type, const_reference);
    constexpr void erase_at_end(pointer);

    pointer   m_data{nullptr};
    size_type m_size{0};
    size_type m_capacity{0};
};

}  // namespace pstd

#include "vector.tpp"
