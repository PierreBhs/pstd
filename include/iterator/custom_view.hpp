#pragma once

#include <ranges>

namespace pstd {

template <std::ranges::view V>
class custom_view : public std::ranges::view_interface<custom_view<V>>
{
public:
    custom_view() = default;
    custom_view(V base) : m_base(std::move(base)) {}

    auto begin() { return std::ranges::begin(m_base); }
    auto end() { return std::ranges::end(m_base); }

private:
    V m_base;
};

// Range adaptor object
struct custom_adaptor : std::ranges::range_adaptor_closure<custom_adaptor>
{
    template <std::ranges::viewable_range R>
    constexpr auto operator()(R&& r) const
    {
        return custom_view(std::views::all(std::forward<R>(r)));
    }
};
}  // namespace pstd
