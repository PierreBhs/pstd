#pragma once

#include <cstddef>
#include <initializer_list>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace pstd {

namespace detail {

template <typename T>
struct is_in_place_type : std::false_type
{};

template <typename T>
struct is_in_place_type<std::in_place_type_t<T>> : std::true_type
{};

template <typename T>
inline constexpr bool is_in_place_type_v = is_in_place_type<T>::value;

}  // namespace detail

class bad_any_cast : public std::bad_cast
{
public:
    const char* what() const noexcept override { return "bad any_cast"; }
};

class any
{
public:
    /* Constructors */

    constexpr any() noexcept = default;
    any(const any&);
    any(any&&) noexcept;

    template <typename T, typename VT = std::decay_t<T>>
        requires(!std::is_same_v<VT, any>) && (!detail::is_in_place_type_v<VT>) && std::is_copy_constructible_v<VT>
    any(T&&);

    template <typename T, typename... Args, typename VT = std::decay_t<T>>
        requires std::is_copy_constructible_v<VT> && std::is_constructible_v<VT, Args...>
    explicit any(std::in_place_type_t<T>, Args&&...);

    template <typename T, typename U, typename... Args, typename VT = std::decay_t<T>>
        requires std::is_copy_constructible_v<VT> && std::is_constructible_v<VT, std::initializer_list<U>&, Args...>
    explicit any(std::in_place_type_t<T>, std::initializer_list<U>, Args&&...);

    /* Destructor */

    ~any();

    /* Assignement */

    any& operator=(const any&);
    any& operator=(any&&) noexcept;

    template <typename T, typename VT = std::decay_t<T>>
        requires(!std::is_same_v<any, VT>) && std::is_copy_constructible_v<VT>
    any& operator=(T&&);

    /* Modifiers */

    template <typename T, typename... Args, typename VT = std::decay_t<T>>
        requires std::is_copy_constructible_v<VT> && std::is_constructible_v<VT, Args...>
    VT& emplace(Args&&...);

    template <typename T, typename U, typename... Args, typename VT = std::decay_t<T>>
        requires std::is_copy_constructible_v<VT> && std::is_constructible_v<VT, std::initializer_list<U>&, Args...>
    VT& emplace(std::initializer_list<U>, Args&&...);

    void reset() noexcept;

    void swap(any& other) noexcept;

    /* Observers */

    [[nodiscard]] bool                  has_value() const noexcept;
    [[nodiscard]] const std::type_info& type() const noexcept;

private:
    static constexpr std::size_t buffer_size = 3 * sizeof(void*);
    static constexpr std::size_t buffer_align = alignof(std::max_align_t);

    union storage {
        alignas(buffer_align) std::byte buffer[buffer_size];
        void* heap;
    };

    struct vtable
    {
        const std::type_info& (*type)() noexcept;
        void (*destroy)(storage&) noexcept;
        void (*copy)(const storage&, storage&);
        void (*move)(storage&, storage&) noexcept;
        void* (*get)(storage&) noexcept;
        const void* (*cget)(const storage&) noexcept;
    };

    template <typename T>
    static constexpr bool fits_in_sbo =
        (sizeof(T) <= buffer_size) && (alignof(T) <= buffer_align) && std::is_nothrow_move_constructible_v<T>;

    template <typename T>
    struct small_handler;

    template <typename T>
    struct large_handler;

    template <typename T>
    using Handler = std::conditional_t<fits_in_sbo<T>, small_handler<T>, large_handler<T>>;

    template <typename T, typename... Args>
    void construct(Args&&... args);

    storage       m_storage{};
    const vtable* m_vtable{nullptr};

    /* Casts */

    template <typename T>
    friend T any_cast(const any&);
    template <typename T>
    friend T any_cast(any&);
    template <typename T>
    friend T any_cast(any&&);
    template <typename T>
    friend const T* any_cast(const any*) noexcept;
    template <typename T>
    friend T* any_cast(any*) noexcept;
};

inline void swap(any& lhs, any& rhs) noexcept;

template <typename T>
T any_cast(const any&);
template <typename T>
T any_cast(any&);
template <typename T>
T any_cast(any&&);
template <typename T>
const T* any_cast(const any*) noexcept;
template <typename T>
T* any_cast(any*) noexcept;

template <typename T, typename... Args>
[[nodiscard]] inline any make_any(Args&&... args);

template <typename T, typename U, typename... Args>
[[nodiscard]] inline any make_any(std::initializer_list<U> il, Args&&... args);

}  // namespace pstd

#include "any.tpp"
