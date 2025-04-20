#include <gtest/gtest.h>
#include <ranges>

#include <pstd/iterator/custom_view.hpp>
#include <pstd/vector/vector.hpp>

TEST(MyViewTest, BasicUsage)
{
    pstd::vector<int> data{1, 2, 3};
    auto              view = data | pstd::custom_adaptor();  // Using the custom adaptor

    // Test if view contains the same elements as data
    auto it = view.begin();
    EXPECT_EQ(*it++, 1);
    EXPECT_EQ(*it++, 2);
    EXPECT_EQ(*it++, 3);
    EXPECT_EQ(it, view.end());
}

TEST(MyViewTest, Composability)
{
    pstd::vector<int> data{1, 2, 3};
    auto              view = data | pstd::custom_adaptor() | std::views::transform([](int x) { return x * 2; });

    // Test transformed values
    std::vector<int> expected{2, 4, 6};
    EXPECT_TRUE(std::ranges::equal(view, expected));
}
