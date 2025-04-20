#include <atomic>

#include <pstd/ptrs/shared_ptr.hpp>

namespace pstd {

template <typename T>
template <typename Deleter, typename Alloc>
shared_ptr<T>::shared_ptr(T* p, Deleter d, Alloc a)
{
    if (!p) {
        return;
    }

    using ControlBlockAlloc = control_block<T, Deleter, Alloc>::alloc_type;

    ControlBlockAlloc ctrl_block_alloc(a);
    auto*             ctrl_block{ctrl_block_alloc.allocate(1)};
    try {
        std::allocator_traits<ControlBlockAlloc>::construct(ctrl_block_alloc, ctrl_block, p, std::move(d), a);
    } catch (...) {
        ctrl_block_alloc.deallocate(ctrl_block, 1);
        throw;
    }

    m_ptr = p;
    m_ctrl_block = ctrl_block;
}

template <typename T>
shared_ptr<T>::shared_ptr(const shared_ptr& other) noexcept : m_ptr(other.m_ptr), m_ctrl_block(other.m_ctrl_block)
{
    if (m_ctrl_block) {
        m_ctrl_block->m_shared_count.fetch_add(1, std::memory_order_relaxed);
    }
}

template <typename T>
shared_ptr<T>::shared_ptr(shared_ptr&& other) noexcept
    : m_ptr(std::exchange(other.m_ptr, nullptr)), m_ctrl_block(std::exchange(other.m_ctrl_block, nullptr))
{}

template <typename T>
shared_ptr<T>::~shared_ptr()
{
    if (m_ctrl_block) {
        if (m_ctrl_block->m_shared_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            m_ctrl_block->dispose();
            m_ctrl_block->on_last_shared();
        }
    }
}

template <typename T>
shared_ptr<T>& shared_ptr<T>::operator=(const shared_ptr& other) noexcept
{
    if (this != &other) {
        reset();
        m_ptr = other.m_ptr;
        m_ctrl_block = other.m_ctrl_block;
        if (m_ctrl_block) {
            m_ctrl_block->m_shared_count.fetch_add(1, std::memory_order_relaxed);
        }
    }
    return *this;
}

template <typename T>
shared_ptr<T>& shared_ptr<T>::operator=(shared_ptr&& other) noexcept
{
    shared_ptr(std::move(other)).swap(*this);
    return *this;
}

template <typename T>
void shared_ptr<T>::reset() noexcept
{
    shared_ptr().swap(*this);
}

template <typename T, typename... Args>
shared_ptr<T> make_shared(Args&&... args)
{
    using Alloc = std::allocator<T>;
    using ControlBlockAlloc = make_shared_control_block<T, Alloc>::alloc_type;

    Alloc             alloc;
    ControlBlockAlloc cb_alloc(alloc);
    auto*             cb = cb_alloc.allocate(1);

    try {
        std::allocator_traits<ControlBlockAlloc>::construct(cb_alloc, cb, alloc, std::forward<Args>(args)...);
    } catch (...) {
        cb_alloc.deallocate(cb, 1);
        throw;
    }

    shared_ptr<T> result;
    result.m_ptr = cb->get_ptr();
    result.m_ctrl_block = cb;
    return result;
}

}  // namespace pstd
