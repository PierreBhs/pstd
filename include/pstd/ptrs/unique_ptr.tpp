#include <pstd/ptrs/unique_ptr.hpp>

namespace pstd {

template <typename T, typename Deleter>
constexpr unique_ptr<T, Deleter>::unique_ptr(unique_ptr&& other) noexcept
    : m_ptr(other.release()), m_deleter(std::move(other.m_deleter))
{}

template <typename T, typename Deleter>
constexpr unique_ptr<T, Deleter>::~unique_ptr()
{
    if (get()) {
        get_deleter()(get());
    }
}

/* Assignment */

template <typename T, typename Deleter>
constexpr unique_ptr<T, Deleter>& unique_ptr<T, Deleter>::operator=(unique_ptr&& other) noexcept
{
    if (this != &other) {
        reset(other.release());
        m_deleter = std::move(other.m_deleter);
    }
    return *this;
}

template <typename T, typename Deleter>
constexpr unique_ptr<T, Deleter>& unique_ptr<T, Deleter>::operator=(std::nullptr_t) noexcept
{
    reset();
    return *this;
}

/* Modifiers */

template <typename T, typename Deleter>
constexpr unique_ptr<T, Deleter>::pointer unique_ptr<T, Deleter>::release() noexcept
{
    pointer p{get()};
    m_ptr = nullptr;
    return p;
}

template <typename T, typename Deleter>
constexpr void unique_ptr<T, Deleter>::reset(pointer p) noexcept
{
    const pointer old_ptr{get()};
    m_ptr = p;
    if (old_ptr) {
        get_deleter()(old_ptr);
    }
}

template <typename T, typename Deleter>
void unique_ptr<T, Deleter>::swap(unique_ptr& rhs) noexcept
{
    using std::swap;
    swap(get(), rhs.get());
    swap(get_deleter(), rhs.get_deleter());
}

}  // namespace pstd
