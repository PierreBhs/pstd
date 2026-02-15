#include <pstd/any/any.hpp>

#include <memory>

namespace pstd {

template <typename T>
struct any::small_handler
{

    static const std::type_info& type() noexcept { return typeid(T); }

    static void destroy(storage& s) noexcept { std::destroy_at(reinterpret_cast<T*>(s.buffer)); }

    static void copy(const storage& src, storage& dest)
    {
        std::construct_at(reinterpret_cast<T*>(dest.buffer), *reinterpret_cast<const T*>(src.buffer));
    }

    static void move(storage& src, storage& dest) noexcept
    {
        auto* src_ptr = reinterpret_cast<T*>(src.buffer);
        std::construct_at(reinterpret_cast<T*>(dest.buffer), std::move(*src_ptr));
        std::destroy_at(src_ptr);
    }

    static void* get(storage& s) noexcept { return s.buffer; }

    static const void* cget(const storage& s) noexcept { return s.buffer; }

    static constexpr vtable table = {type, destroy, copy, move, get, cget};
};

template <typename T>
struct any::large_handler
{
    static const std::type_info& type() noexcept { return typeid(T); }

    static void destroy(storage& s) noexcept { delete static_cast<T*>(s.heap); }

    static void copy(const storage& src, storage& dest) { dest.heap = new T(*static_cast<T*>(src.heap)); }

    static void move(storage& src, storage& dest) noexcept
    {
        dest.heap = src.heap;
        src.heap = nullptr;
    }

    static void* get(storage& s) noexcept { return s.heap; }

    static const void* cget(const storage& s) noexcept { return s.heap; }

    static constexpr vtable table = {type, destroy, copy, move, get, cget};
};

template <typename T, typename... Args>
void any::construct(Args&&... args)
{
    using H = Handler<T>;

    if constexpr (fits_in_sbo<T>) {
        std::construct_at(reinterpret_cast<T*>(m_storage.buffer), std::forward<Args>(args)...);
    } else {
        m_storage.heap = new T(std::forward<Args>(args)...);
    }

    m_vtable = &H::table;
}

inline any::any(const any& other)
{
    if (other.has_value()) {
        other.m_vtable->copy(other.m_storage, m_storage);
        m_vtable = other.m_vtable;
    }
}

inline any::any(any&& other) noexcept
{
    if (other.has_value()) {
        other.m_vtable->move(other.m_storage, m_storage);
        m_vtable = other.m_vtable;
        other.m_vtable = nullptr;
    }
}

template <typename T, typename VT>
    requires(!std::is_same_v<VT, any>) && (!detail::is_in_place_type_v<VT>) && std::is_copy_constructible_v<VT>
any::any(T&& value)
{
    construct<VT>(std::forward<T>(value));
}

template <typename T, typename... Args, typename VT>
    requires std::is_copy_constructible_v<VT> && std::is_constructible_v<VT, Args...>
any::any(std::in_place_type_t<T>, Args&&... args)
{
    construct<VT>(std::forward<Args&&>(args)...);
}

template <typename T, typename U, typename... Args, typename VT>
    requires std::is_copy_constructible_v<VT> && std::is_constructible_v<VT, std::initializer_list<U>&, Args...>
any::any(std::in_place_type_t<T>, std::initializer_list<U> il, Args&&... args)
{
    construct<VT>(il, std::forward<Args&&>(args)...);
}

inline any::~any()
{
    reset();
}

inline any& any::operator=(const any& rhs)
{
    any(rhs).swap(*this);
    return *this;
}

inline any& any::operator=(any&& rhs) noexcept
{
    any(std::move(rhs)).swap(*this);
    return *this;
}

template <typename T, typename VT>
    requires(!std::is_same_v<any, VT>) && std::is_copy_constructible_v<VT>
any& any::operator=(T&& rhs)
{
    any(std::forward<T>(rhs)).swap(*this);
    return *this;
}

template <typename T, typename... Args, typename VT>
    requires std::is_copy_constructible_v<VT> && std::is_constructible_v<VT, Args...>
VT& any::emplace(Args&&... args)
{
    reset();
    construct<VT>(std::forward<Args>(args)...);

    return *static_cast<VT*>(m_vtable->get(m_storage));
}

template <typename T, typename U, typename... Args, typename VT>
    requires std::is_copy_constructible_v<VT> && std::is_constructible_v<VT, std::initializer_list<U>&, Args...>
VT& any::emplace(std::initializer_list<U> il, Args&&... args)
{
    reset();
    construct<VT>(il, std::forward<Args>(args)...);

    return *static_cast<VT*>(m_vtable->get(m_storage));
}

inline void any::reset() noexcept
{
    if (has_value()) {
        m_vtable->destroy(m_storage);
        m_vtable = nullptr;
    }
}

inline void any::swap(any& other) noexcept
{
    if (this == &other) {
        return;
    }

    if (has_value() && other.has_value()) {
        any tmp;
        other.m_vtable->move(other.m_storage, tmp.m_storage);
        tmp.m_vtable = other.m_vtable;
        other.m_vtable = nullptr;

        m_vtable->move(m_storage, other.m_storage);
        other.m_vtable = m_vtable;
        m_vtable = nullptr;

        tmp.m_vtable->move(tmp.m_storage, m_storage);
        m_vtable = tmp.m_vtable;
        tmp.m_vtable = nullptr;
    } else if (has_value()) {
        m_vtable->move(m_storage, other.m_storage);
        other.m_vtable = m_vtable;
        m_vtable = nullptr;
    } else if (other.has_value()) {
        other.m_vtable->move(other.m_storage, m_storage);
        m_vtable = other.m_vtable;
        other.m_vtable = nullptr;
    }
}

[[nodiscard]] inline bool any::has_value() const noexcept
{
    return m_vtable != nullptr;
}

[[nodiscard]] inline const std::type_info& any::type() const noexcept
{
    return has_value() ? m_vtable->type() : typeid(void);
}

inline void swap(any& lhs, any& rhs) noexcept
{
    lhs.swap(rhs);
}

template <typename T>
T* any_cast(any* operand) noexcept
{
    static_assert(!std::is_void_v<T>, "T must not be void");
    static_assert(!std::is_reference_v<T>, "T may not be a reference.");

    if (operand == nullptr || operand->type() != typeid(T)) {
        return nullptr;
    }
    return static_cast<T*>(operand->m_vtable->get(operand->m_storage));
}

template <typename T>
const T* any_cast(const any* operand) noexcept
{
    static_assert(!std::is_void_v<T>, "T must not be void");
    static_assert(!std::is_reference_v<T>, "T may not be a reference.");

    if (operand == nullptr || operand->type() != typeid(T)) {
        return nullptr;
    }
    return static_cast<const T*>(operand->m_vtable->cget(operand->m_storage));
}

template <typename T>
T any_cast(const any& operand)
{
    using U = std::remove_cvref_t<T>;
    static_assert(std::is_constructible_v<T, const U&>,
                  "any_cast<T>(const any&): T must be constructible from const U&");

    const auto* ptr = any_cast<U>(&operand);
    if (ptr == nullptr) {
        throw bad_any_cast{};
    }
    return static_cast<T>(*ptr);
}

template <typename T>
T any_cast(any& operand)
{
    using U = std::remove_cvref_t<T>;
    static_assert(std::is_constructible_v<T, U&>, "any_cast<T>(any&): T must be constructible from U&");

    auto* ptr = any_cast<U>(&operand);
    if (ptr == nullptr) {
        throw bad_any_cast{};
    }
    return static_cast<T>(*ptr);
}

template <typename T>
T any_cast(any&& operand)
{
    using U = std::remove_cvref_t<T>;
    static_assert(std::is_constructible_v<T, U>, "any_cast<T>(any&&): T must be constructible from U");

    auto* ptr = any_cast<U>(&operand);
    if (ptr == nullptr) {
        throw bad_any_cast{};
    }
    return static_cast<T>(std::move(*ptr));
}

template <typename T, typename... Args>
inline any make_any(Args&&... args)
{
    return any(std::in_place_type<T>, std::forward<Args>(args)...);
}

template <typename T, typename U, typename... Args>
inline any make_any(std::initializer_list<U> il, Args&&... args)
{
    return any(std::in_place_type<T>, il, std::forward<Args>(args)...);
}

}  // namespace pstd
