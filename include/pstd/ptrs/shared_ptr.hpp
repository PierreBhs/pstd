#pragma once

#include <atomic>
#include <memory>
#include <type_traits>
#include <utility>

namespace pstd {

// Different control blocks needed for objects created via make_shared / in-place / raw ptrs
// Interface used to follow the std lib's implementation notes (see cppreference)
//  1. make_shared&allocated_shared -> memory for both the control block and the managed object in a single alloc
//     Created in-place in a data member of the control block
//  2. Via shared_ptr's ctors -> must allocated seperately (control block stores a pointer to the managed object)
struct control_block_base
{
    control_block_base(std::size_t initial_shared, std::size_t initial_weak)
        : m_shared_count(initial_shared), m_weak_count(initial_weak)
    {}

    virtual ~control_block_base() = default;

    virtual void dispose() noexcept = 0;
    virtual void destroy() noexcept = 0;
    virtual void on_last_shared() noexcept = 0;  // handle weak_ptr case

    std::atomic<std::size_t> m_shared_count;
    std::atomic<std::size_t> m_weak_count;
};

template <typename T, typename Deleter, typename Alloc>
class control_block final : public control_block_base
{
public:
    using alloc_type = typename std::allocator_traits<Alloc>::template rebind_alloc<control_block>;

    control_block(T* ptr, Deleter d, Alloc a)
        : control_block_base(1, 1), m_ptr(ptr), m_deleter(std::move(d)), m_alloc(std::move(a))
    {}

    void dispose() noexcept override { m_deleter(m_ptr); }
    void destroy() noexcept override
    {
        alloc_type cb_alloc(m_alloc);
        std::allocator_traits<alloc_type>::destroy(cb_alloc, this);
        std::allocator_traits<alloc_type>::deallocate(cb_alloc, this, 1);
    }

    void on_last_shared() noexcept override
    {
        // Only destroy when no more weak_ptr, otherwise keep alive until weak_ptr gets destructed
        if (m_weak_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            destroy();
        }
    }

private:
    T*      m_ptr;
    Deleter m_deleter;
    Alloc   m_alloc;
};

template <typename T, typename Alloc>
class make_shared_control_block final : public control_block_base
{

public:
    using alloc_type = typename std::allocator_traits<Alloc>::template rebind_alloc<make_shared_control_block>;
    template <typename... Args>
    make_shared_control_block(Alloc a, Args&&... args) : control_block_base(1, 0), m_alloc(std::move(a))
    {
        new (static_cast<void*>(m_storage)) T(std::forward<Args>(args)...);
    }

    ~make_shared_control_block() = default;

    void dispose() noexcept override { std::destroy_at(get_ptr()); }

    void destroy() noexcept override
    {
        this->~make_shared_control_block();
        ::operator delete(this);
    }

    void on_last_shared() noexcept override
    {
        if (m_weak_count.load(std::memory_order_acquire) == 0) {
            destroy();
        }
    }

    T* get_ptr() noexcept { return reinterpret_cast<T*>(m_storage); }

private:
    static constexpr size_t alignment = alignof(T);
    static constexpr size_t size = sizeof(T);

    // C++23 compliant storage
    alignas(alignment) std::byte m_storage[size];
    Alloc m_alloc;
};

template <typename T>
class shared_ptr
{
public:
    using element_type = std::remove_extent<T>;

    constexpr shared_ptr() noexcept = default;
    constexpr shared_ptr(std::nullptr_t) noexcept : shared_ptr() {}

    template <typename Deleter = std::default_delete<T>, typename Alloc = std::allocator<T>>
    explicit shared_ptr(T* p, Deleter d = Deleter(), Alloc a = Alloc());

    shared_ptr(const shared_ptr& other) noexcept;
    shared_ptr(shared_ptr&& other) noexcept;

    ~shared_ptr();

    shared_ptr& operator=(const shared_ptr&) noexcept;
    shared_ptr& operator=(shared_ptr&&) noexcept;

    void reset() noexcept;

    inline T*   get() const noexcept { return m_ptr; }
    inline T&   operator*() const noexcept { return *get(); }
    inline T*   operator->() const noexcept { return get(); }
    inline long use_count() const noexcept
    {
        return m_ctrl_block ? m_ctrl_block->m_shared_count.load(std::memory_order_relaxed) : 0;
    }
    explicit operator bool() const noexcept { return get() != nullptr; }

private:
    template <typename U, typename... Args>
    friend shared_ptr<U> make_shared(Args&&... args);

    inline void swap(shared_ptr& other) noexcept
    {
        std::swap(m_ptr, other.m_ptr);
        std::swap(m_ctrl_block, other.m_ctrl_block);
    }

    T*                  m_ptr{nullptr};
    control_block_base* m_ctrl_block{nullptr};
};

template <typename T, typename... Args>
shared_ptr<T> make_shared(Args&&... args);

}  // namespace pstd

#include "shared_ptr.tpp"
