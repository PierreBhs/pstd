#pragma once

#include <cstddef>
#include <iterator>

// std::iterator deprecated in C++17

namespace pstd {

template <typename T>
class iterator
{
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    iterator(T* p = nullptr) : ptr(p) {}

    iterator& operator++()
    {
        ++ptr;
        return *this;
    }

    iterator operator++(int)
    {
        auto tmp{*this};
        ++ptr;
        return tmp;
    }

    reference operator*() const { return *ptr; }
    pointer   operator->() const { return ptr; }

    bool operator==(const iterator&) const = default;

private:
    T* ptr;
};

template <typename T>
class random_iterator
{
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    random_iterator(T* p = nullptr) : m_ptr(p) {}

    random_iterator& operator++()
    {
        ++m_ptr;
        return *this;
    }

    random_iterator operator++(int)
    {
        auto tmp{*this};
        ++m_ptr;
        return tmp;
    }

    random_iterator& operator--()
    {
        --m_ptr;
        return *this;
    }

    random_iterator operator--(int)
    {
        auto tmp{*this};
        --m_ptr;
        return tmp;
    }

    random_iterator& operator+=(difference_type n)
    {
        m_ptr += n;
        return *this;
    }

    random_iterator& operator-=(difference_type n)
    {
        m_ptr -= n;
        return *this;
    }

    friend random_iterator operator+(random_iterator it, difference_type n) { return it += n; }
    friend random_iterator operator+(difference_type n, random_iterator it) { return it += n; }
    friend random_iterator operator-(random_iterator it, difference_type n) { return it -= n; }

    friend difference_type operator-(const random_iterator& a, const random_iterator& b) { return a.m_ptr - b.m_ptr; }

    reference operator[](difference_type n) const { return m_ptr[n]; }

    reference operator*() const { return *m_ptr; }
    pointer   operator->() const { return *m_ptr; }

    auto operator<=>(const random_iterator&) const = default;

private:
    T* m_ptr;
};

}  // namespace pstd
