#include "vector/vector.hpp"

#include "mem_helpers.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <memory>

#include <print>

namespace {

/* Find next power of two for growing capacity */
constexpr uint64_t next_pow2(uint64_t x)
{
    if (x <= 1) [[unlikely]] {
        return 2;
    }

    for (int i = 0; i < 63; i++)
        x |= x >> 1;
    return ++x;
}

}  // namespace

namespace pstd {

template <typename T>
constexpr vector<T>::vector() noexcept : m_data{nullptr}, m_size{0}, m_capacity{0}
{}

template <typename T>
constexpr vector<T>::vector(size_type n) : m_data{nullptr}, m_size{n}, m_capacity{next_pow2(m_size)}
{
    m_data = static_cast<T*>(operator new(sizeof(value_type) * m_capacity));
    std::uninitialized_default_construct_n(begin(), n);
}

template <typename T>
constexpr vector<T>::vector(size_type n, const_reference val)
    : m_data{nullptr}, m_size{n}, m_capacity{next_pow2(m_size)}
{
    m_data = static_cast<T*>(operator new(sizeof(value_type) * m_capacity));
    std::uninitialized_fill_n(begin(), n, val);
}

template <typename T>
constexpr vector<T>::vector(const vector& other) : m_data(nullptr), m_size(other.size()), m_capacity(other.capacity())
{
    m_data = static_cast<T*>(operator new(sizeof(value_type) * m_capacity));
    std::uninitialized_copy_n(other.begin(), m_size, begin());
}

template <typename T>
template <typename InputIt>
constexpr vector<T>::vector(InputIt first, InputIt last)
    : m_data(nullptr), m_size(std::distance(first, last)), m_capacity(next_pow2(m_size))
{
    m_data = static_cast<T*>(operator new(sizeof(value_type) * m_capacity));
    std::uninitialized_move_n(first, m_size, begin());
}

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
    destroy();
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

template <typename T>
constexpr T& vector<T>::front()
{
    return *begin();
}

template <typename T>
constexpr const T& vector<T>::front() const
{
    return *begin();
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
constexpr void vector<T>::clear() noexcept
{
    static_cast<void>(std::destroy_n(begin(), size()));
    m_size = 0;
}

template <typename T>
constexpr vector<T>::iterator vector<T>::insert(const_iterator pos, const T& val)
    requires(std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>)
{
    // Need to return an iterator from the const_iterator input
    size_type offset{static_cast<size_type>(pos - cbegin())};

    if (size() == capacity()) {
        m_capacity = next_pow2(capacity());
        m_data = grow(capacity(), true);
    }

    for (size_type i{size()}; i > offset; --i) {
        m_data[i] = m_data[i - 1];
    }

    m_data[offset] = val;
    m_size++;

    return begin() + offset;
}

template <typename T>
constexpr vector<T>::iterator vector<T>::insert(const_iterator pos, T&& value)
    requires(std::is_move_constructible_v<T> && std::is_move_assignable_v<T>)
{
    size_type offset{static_cast<size_type>(pos - cbegin())};

    if (size() == capacity()) {
        m_capacity = next_pow2(capacity());
        m_data = grow(capacity(), false);
    }

    for (size_type i{size()}; i > offset; --i) {
        m_data[i] = std::move(m_data[i - 1]);  // std::move ? Or done automatically on movable objects ?
    }

    m_data[offset] = std::move(value);
    m_size++;

    return begin() + offset;
}

template <typename T>
template <typename... Args>
constexpr vector<T>::reference vector<T>::emplace_back(Args&&... args)
    requires std::move_constructible<T>
{
    if (size() == capacity()) {
        m_capacity = next_pow2(capacity());
        m_data = grow(capacity(), false);
    }

    ::new (static_cast<T*>(std::addressof(*(m_data + size())))) T(std::forward<Args>(args)...);
    m_size++;

    return m_data[m_size - 1];
}

template <typename T>
constexpr void vector<T>::push_back(const_reference value)
    requires std::copy_constructible<T>
{
    if (size() == capacity()) {
        m_capacity = next_pow2(capacity());
        m_data = grow(capacity(), true);
    }

    ::new (static_cast<T*>(std::addressof(*(m_data + size())))) T(value);
    m_size++;
}

template <typename T>
constexpr void vector<T>::push_back(T&& value)
    requires std::move_constructible<T>
{
    if (size() == capacity()) {
        m_capacity = next_pow2(capacity());
        m_data = grow(capacity(), false);
    }

    ::new (static_cast<T*>(std::addressof(*(m_data + size())))) T(std::move(value));
    m_size++;
}

template <typename T>
constexpr void vector<T>::pop_back()
{
    if (empty()) {
        return;
    }

    m_size--;
    std::destroy_at(end());
}

template <typename T>
constexpr void vector<T>::resize(size_type count)
    requires(std::is_move_constructible_v<T> && std::is_default_constructible_v<T>)
{
    if (count > size()) {
        default_append(count - size());
    } else if (count < size()) {
        erase_at_end(begin() + count);
    }
}

template <typename T>
constexpr void vector<T>::resize(size_type count, const_reference value)
    requires std::is_copy_constructible_v<T>
{
    if (count > size()) {
        append(count - size(), value);
    } else if (count < size()) {
        erase_at_end(begin() + count);
    }
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
constexpr T* vector<T>::grow(size_type new_capacity, bool copy)
{
    T* new_data{allocate(new_capacity)};

    if (copy) {
        std::uninitialized_copy_n(begin(), size(), new_data);
    } else {
        std::uninitialized_move_n(begin(), size(), new_data);
    }

    // Iterator invalidation
    destroy();

    return new_data;
}

template <typename T>
constexpr T* vector<T>::allocate(size_type n)
{
    return static_cast<T*>(::operator new(sizeof(value_type) * n));
}

template <typename T>
constexpr void vector<T>::destroy()
{
    std::destroy_n(begin(), size());
    ::operator delete(m_data);
}

template <typename T>
constexpr void vector<T>::default_append(size_type n)
{
    if (n == 0) {
        return;
    }

    const auto curr_size{size()};
    const auto available_size{capacity() - size()};

    if (size() > max_size() || available_size > max_size() - curr_size) {
        return;  // unreachable;
    }

    // Append to the end if sufficient space, else grow the vector
    if (available_size >= n) {
        // Would std::uninitialized_value_construct_n work better here ?
        pstd::mem::uninitialized_default_construct_n(end(), n);
        m_size += n;
    } else {
        const auto new_len{curr_size + n};
        const auto new_cap{next_pow2(new_len)};
        auto*      new_data{allocate(new_cap)};

        pstd::mem::uninitialized_default_construct_n(new_data + curr_size, n);
        std::uninitialized_move_n(begin(), curr_size, new_data);

        destroy();

        m_data = new_data;
        m_size = new_len;
        m_capacity = new_cap;
    }
}

template <typename T>
constexpr void vector<T>::append(size_type n, const_reference val)
{
    if (n == 0) {
        return;
    }

    const auto curr_size{size()};
    const auto available_size{capacity() - size()};

    if (size() > max_size() || available_size > max_size() - curr_size) {
        return;  // unreachable;
    }

    // Append to the end if sufficient space, else grow the vector
    if (available_size >= n) {
        std::uninitialized_fill_n(end(), n, val);
        m_size += n;
    } else {
        const auto new_len{curr_size + n};
        const auto new_cap{next_pow2(new_len)};
        auto*      new_data{allocate(new_cap)};

        std::uninitialized_fill_n(new_data + curr_size, n, val);
        std::uninitialized_move_n(begin(), curr_size, new_data);  // copy_n ?

        destroy();

        m_data = new_data;
        m_size = new_len;
        m_capacity = new_cap;
    }
}

template <typename T>
constexpr void vector<T>::erase_at_end(pointer pos)
{
    if (size_type n = end() - pos) {
        std::destroy_n(pos, n);
        m_size = pos - begin();  // hopefully not modified in between ? >:(
    }
}

// Non-member functions

template <class T>
bool operator==(const vector<T>& lhs, const vector<T>& rhs)
{
    if (lhs.size() != rhs.size()) {
        return false;
    }

    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
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
