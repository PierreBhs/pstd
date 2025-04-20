#pragma once

#include <concepts>
#include <memory>

namespace pstd {

template <typename T, typename Deleter = std::default_delete<T>>
class unique_ptr
{
public:
    using pointer = T*;
    using element_type = T;
    using deleter_type = Deleter;

    /* Constructors */

    constexpr unique_ptr()
        requires std::default_initializable<Deleter>
        : m_ptr(nullptr), m_deleter()
    {}

    constexpr explicit unique_ptr(pointer p) noexcept : m_ptr(p), m_deleter() {}

    constexpr unique_ptr(pointer p, const deleter_type& d) noexcept : m_ptr(p), m_deleter(d) {}
    constexpr unique_ptr(pointer p, deleter_type&& d) noexcept : m_ptr(p), m_deleter(std::move(d)) {}

    constexpr unique_ptr(unique_ptr&&) noexcept;
    template <typename U, typename E>
        requires std::convertible_to<typename unique_ptr<U, E>::pointer, pointer> &&
                 std::convertible_to<E, deleter_type>
    unique_ptr(unique_ptr<U, E>&&) noexcept;

    unique_ptr(const unique_ptr&) = delete;

    constexpr ~unique_ptr();

    /* Assignment */

    constexpr unique_ptr& operator=(unique_ptr&&) noexcept;
    constexpr unique_ptr& operator=(std::nullptr_t) noexcept;
    unique_ptr& operator=(const unique_ptr&) = delete;

    /* Observers */

    constexpr pointer   get() const noexcept { return m_ptr; }
    deleter_type&       get_deleter() noexcept { return m_deleter; }
    const deleter_type& get_deleter() const noexcept { return m_deleter; }
    explicit            operator bool() const noexcept { return get() != nullptr; }

    /* Modifiers */

    constexpr pointer release() noexcept;
    constexpr void    reset(pointer p = pointer()) noexcept;
    void              swap(unique_ptr&) noexcept;

    /* Accessors */

    T&      operator*() const noexcept { return *m_ptr; }
    pointer operator->() const noexcept { return m_ptr; }

private:
    pointer      m_ptr{nullptr};
    deleter_type m_deleter;
};

template <typename T, typename... Args>
constexpr inline unique_ptr<T> make_unique(Args&&... args)
{
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

}  // namespace pstd

#include "unique_ptr.tpp"
