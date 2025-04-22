#include <pstd/heap/binary_heap.hpp>

#include <algorithm>
#include <utility>

namespace pstd {
template <typename T, typename Container, typename Compare>
void binary_heap<T, Container, Compare>::sift_up(size_type index)
{
    while (index > 0) {
        size_type parent_index = (index - 1) / 2;
        // If child has higher priority than parent according to comp
        if (m_comp(m_c[parent_index], m_c[index])) {
            using std::swap;  // Enable ADL
            swap(m_c[index], m_c[parent_index]);
            index = parent_index;  // Move up to the parent's position
        } else {
            break;  // Heap property satisfied for this subtree path
        }
    }
}

template <typename T, typename Container, typename Compare>
void binary_heap<T, Container, Compare>::sift_down(size_type index)
{
    size_type heap_size = m_c.size();
    while (true) {
        size_type left_child_index = 2 * index + 1;
        size_type right_child_index = 2 * index + 2;
        size_type priority_child_index = index;  // Assume current node has highest priority initially

        if (left_child_index < heap_size && m_comp(m_c[priority_child_index], m_c[left_child_index])) {
            priority_child_index = left_child_index;
        }

        if (right_child_index < heap_size && m_comp(m_c[priority_child_index], m_c[right_child_index])) {
            priority_child_index = right_child_index;
        }

        // If a child had higher priority, swap and continue sifting down
        if (priority_child_index != index) {
            using std::swap;  // ADL
            swap(m_c[index], m_c[priority_child_index]);
            index = priority_child_index;  // Move down to the child's position
        } else {
            break;  // Heap property satisfied for this subtree
        }
    }
}

template <typename T, typename Container, typename Compare>
void binary_heap<T, Container, Compare>::heapify()
{
    // Start from the last non-leaf node and sift down
    if (size() < 2)
        return;  // No need to heapify for 0 or 1 element

    // Start from the parent of the last element
    for (size_type i = (size() / 2) - 1; i != static_cast<size_type>(-1); --i) {
        sift_down(i);
    }

    // Handle potential wrap-around for size_type 0
    if (size() >= 2) {
        sift_down(0);
    }
}

// --- Constructors ---

template <typename T, typename Container, typename Compare>
binary_heap<T, Container, Compare>::binary_heap(const Compare& compare) : m_comp(compare)
{}

template <typename T, typename Container, typename Compare>
binary_heap<T, Container, Compare>::binary_heap(const Compare& compare, const Container& cont)
    : m_c(cont), m_comp(compare)
{
    heapify();
}

template <typename T, typename Container, typename Compare>
binary_heap<T, Container, Compare>::binary_heap(const Compare& compare, Container&& cont)
    : m_c(std::move(cont)), m_comp(compare)
{
    heapify();
}

template <typename T, typename Container, typename Compare>
template <std::input_iterator InputIt>
binary_heap<T, Container, Compare>::binary_heap(InputIt first, InputIt last, const Compare& compare)
    : m_c(first, last), m_comp(compare)
{
    heapify();
}

// --- Element Access ---

template <typename T, typename Container, typename Compare>
[[nodiscard]] typename binary_heap<T, Container, Compare>::const_reference binary_heap<T, Container, Compare>::top()
    const
{
    assert(!empty() && "Cannot call top() on an empty binary_heap");
    return m_c.front();
}

// --- Capacity ---

template <typename T, typename Container, typename Compare>
[[nodiscard]] bool binary_heap<T, Container, Compare>::empty() const noexcept
{
    return m_c.empty();
}

template <typename T, typename Container, typename Compare>
[[nodiscard]] typename binary_heap<T, Container, Compare>::size_type binary_heap<T, Container, Compare>::size()
    const noexcept
{
    return m_c.size();
}

// --- Modifiers ---

template <typename T, typename Container, typename Compare>
void binary_heap<T, Container, Compare>::push(const value_type& value)
{
    m_c.push_back(value);
    sift_up(m_c.size() - 1);
}

template <typename T, typename Container, typename Compare>
void binary_heap<T, Container, Compare>::push(value_type&& value)
{
    m_c.push_back(std::move(value));
    sift_up(m_c.size() - 1);
}

template <typename T, typename Container, typename Compare>
template <typename... Args>
void binary_heap<T, Container, Compare>::emplace(Args&&... args)
{
    m_c.emplace_back(std::forward<Args>(args)...);
    sift_up(m_c.size() - 1);
}

template <typename T, typename Container, typename Compare>
void binary_heap<T, Container, Compare>::pop()
{
    assert(!empty() && "Cannot call pop() on an empty binary_heap");
    if (m_c.size() > 1) {
        using std::swap;
        swap(m_c.front(), m_c.back());  // Move highest priority element to the end
    }
    m_c.pop_back();  // Remove it
    if (!m_c.empty()) {
        sift_down(0);  // Restore heap property from the root
    }
}

template <typename T, typename Container, typename Compare>
void binary_heap<T, Container, Compare>::swap(binary_heap& other) noexcept(std::is_nothrow_swappable_v<Container> &&
                                                                           std::is_nothrow_swappable_v<Compare>)
{
    using std::swap;
    swap(m_c, other.m_c);
    swap(m_comp, other.m_comp);
}

}  // namespace pstd
