#pragma once

#include <cstddef>
#include <initializer_list>

namespace pstd {

template <typename T>
class vector
{
public:
    /* C++ Standard Named Requirements for Containers */
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using iterator = T*;
    using const_iterator = const T*;
    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;

    /* Constructors */

    constexpr vector() noexcept;
    constexpr vector(std::initializer_list<T>);

    constexpr ~vector();

    /* Element access */

    [[nodiscard]] constexpr reference       operator[](size_type);
    [[nodiscard]] constexpr const_reference operator[](size_type) const;

    [[nodiscard]] constexpr reference       front();
    [[nodiscard]] constexpr const_reference front() const;

    [[nodiscard]] constexpr reference       back();
    [[nodiscard]] constexpr const_reference back() const;

    /* Capacity */

    //TODO: Switch to begin() == end()
    [[nodiscard]] constexpr inline bool      empty() const { return m_size == 0; }
    [[nodiscard]] constexpr inline size_type size() const { return m_size; }
    [[nodiscard]] constexpr inline size_type capacity() const { return m_capacity; }

    /* Modifiers */

    constexpr void push_back(const_reference);
    // constexpr void push_back(T&& value);

private:
    T* grow(size_type new_capacity, bool copy);

    T*        m_data{nullptr};
    size_type m_size{0};
    size_type m_capacity{0};
};

}  // namespace pstd

#include "vector.tpp"
