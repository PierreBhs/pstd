#pragma once

#include <memory>
#include <type_traits>

namespace pstd::mem {

template <typename ForwardIterator, typename Size>
inline constexpr void uninitialized_default_construct_n(ForwardIterator first, Size n)
{
    // remove_pointer_t better ?
    using value = typename std::iterator_traits<ForwardIterator>::value_type;
    // Since no ASAN, will generate warnings for trivial types
    // (unitialized values since we just 0 the mem here)
    if constexpr (std::is_trivial_v<value>) {
        std::uninitialized_value_construct_n(first, n);
    } else {
        std::uninitialized_default_construct_n(first, n);
    }
}

}  // namespace pstd::mem
