// Uncomment when building
#include "vector/vector.hpp"

#include <cstring>

#include <print>

namespace pstd {

/* Find next power of two for growing capacity */
uint64_t next_pow2(uint64_t x)
{
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
{
    if (m_size == m_capacity) {
        std::println("{0} {1}", m_capacity, next_pow2(m_capacity));
        m_capacity = next_pow2(m_capacity);

        auto new_data = grow(m_capacity, true);

        // Deallocate old data
        for (size_type i{0ul}; i < m_size; ++i) {
            m_data[i].~T();
        }
        ::operator delete(m_data);

        m_data = new_data;
    }

    m_data[m_size++] = value;
}

/*
** Private helpers
*/
template <typename T>
T* vector<T>::grow(size_type new_capacity, bool copy)
{
    T* new_data = static_cast<T*>(::operator new(new_capacity * sizeof(T)));

    if (copy) {
        for (size_type copied{0ul}; copied < m_size; ++copied) {
            ::new (static_cast<T*>(new_data + copied)) T(*(m_data + copied));
        }
    }

    return new_data;
}
}  // namespace pstd
