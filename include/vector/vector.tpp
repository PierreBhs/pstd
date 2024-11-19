// Uncomment when building
#include "vector/vector.hpp"

#include <cstring>
#include <memory>

#include <print>

namespace pstd {

/* Find next power of two for growing capacity */
uint64_t next_pow2(uint64_t x)
{
    if (x <= 1) [[unlikely]] {
        return 2;
    }

    for (int i = 0; i < 63; i++)
        x |= x >> 1;
    return ++x;
}

template <typename T>
constexpr vector<T>::vector() noexcept : m_data{nullptr}, m_size{0}, m_capacity{0}
{}

template <typename T>
constexpr vector<T>::vector(std::initializer_list<T> init)
    : m_data{nullptr}, m_size{init.size()}, m_capacity{init.size()}
{
    m_data = static_cast<T*>(operator new(sizeof(value_type) * m_capacity));

    size_type index{0ul};
    for (const auto& elem : init) {
        new (m_data + index++) T(elem);
    }
}

template <typename T>
constexpr vector<T>::~vector()
{
    if (m_data == nullptr) {
        return;
    }

    for (size_type i = 0; i < m_size; ++i) {
        (m_data + i)->~T();
    }
    operator delete(m_data);

    m_size = 0;
    m_capacity = 0;
}

/*
**  Element Access
*/

template <typename T>
constexpr T& vector<T>::operator[](size_type pos)
{
    return m_data[pos];
}

template <typename T>
constexpr const T& vector<T>::operator[](size_type pos) const
{
    return m_data[pos];
}

// Calling front on an empty container causes undefined behavior
template <typename T>
constexpr T& vector<T>::front()
{
    return m_data[0];
}

template <typename T>
constexpr const T& vector<T>::front() const
{
    return m_data[0];
}

template <typename T>
constexpr T& vector<T>::back()
{
    return m_data[m_size - 1];
}

template <typename T>
constexpr const T& vector<T>::back() const
{
    return m_data[m_size - 1];
}

/*
**  Modifiers
*/

template <typename T>
constexpr void vector<T>::push_back(const_reference value)
    requires std::copy_constructible<T>
{
    std::print("push_back(const T&): {0} {1}\n", m_size, m_capacity);
    if (m_size == m_capacity) {
        m_capacity = next_pow2(m_capacity);
        m_data = grow(m_capacity, true);
    }

    ::new (static_cast<T*>(std::addressof(*(m_data + size())))) T(value);
    m_size++;
}

template <typename T>
constexpr void vector<T>::push_back(T&& value)
    requires std::move_constructible<T>
{
    std::print("push_back(T&&): {0} {1}\n", m_size, m_capacity);
    if (m_size == m_capacity) {
        m_capacity = next_pow2(m_capacity);
        m_data = grow(m_capacity, false);
    }

    ::new (static_cast<T*>(std::addressof(*(m_data + size())))) T(std::move(value));
    m_size++;
}

/*
** Capacity
*/

template <typename T>
constexpr void vector<T>::reserve(size_type new_cap)
{
    if (new_cap > m_capacity) {
        m_data = grow(new_cap, false);
    }
}

/*
** Private helpers
*/
template <typename T>
T* vector<T>::grow(size_type new_capacity, bool copy)
{
    T* new_data = static_cast<T*>(::operator new(new_capacity * sizeof(T)));

    if (copy) {
        std::uninitialized_copy_n(begin(), size(), new_data);
    } else {
        std::uninitialized_move_n(begin(), size(), new_data);
    }

    // Iterator invalidation
    static_cast<void>(std::destroy_n(begin(), size()));
    operator delete(m_data);

    return new_data;
}
}  // namespace pstd

// if (copy) {
//     std::uninitialized_copy_n(begin(), size(), new_data);
//     // Equivalent to:
//     // for (size_type copied{0ul}; copied < m_size; ++copied) {
//     //     ::new (static_cast<T*>(new_data + copied)) T(*(m_data + copied));
//     // }
// } else {
//     std::uninitialized_move_n(begin(), size(), new_data);
// }

// // Iterator invalidation
// static_cast<void>(std::destroy_n(begin(), size()));
// operator delete(m_data);

// // equiv to:
// // for (size_type i{0ul}; i < m_size; ++i) {
// //     m_data[i].~T();
// // }
// // ::operator delete(m_data);
