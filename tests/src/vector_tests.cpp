#include <gtest/gtest.h>
#include <algorithm>
#include <random>
#include <string>

#include "vector/vector.hpp"

auto generate_random_vector(std::size_t n, int lowerBound, int upperBound)
{
    std::random_device rd;
    std::mt19937       gen{rd()};

    std::uniform_int_distribution<int> dist{lowerBound, upperBound};

    pstd::vector<int> result(n);
    std::ranges::generate(result, [&] { return dist(gen); });

    return result;
}

struct custom_struct
{
    float       x;
    std::string s;

    bool operator==(const custom_struct&) const = default;
};

template <typename T>
class vector_test : public ::testing::Test
{};

using test_t = ::testing::Types<int, std::string, custom_struct>;

template <typename T>
pstd::vector<T> createTestValues()
{
    if constexpr (std::is_same_v<T, int>) {
        return pstd::vector<int>{1, 2, 3, 4, 5, 6};
    } else if constexpr (std::is_same_v<T, std::string>) {
        return pstd::vector<std::string>{"one", "two", "three", "four", "five", "six"};
    } else {
        return pstd::vector<custom_struct>{{1, "first"}, {2, "second"}, {3, "third"}};
    }
}

TYPED_TEST_SUITE(vector_test, test_t);

TYPED_TEST(vector_test, default_constructor)
{
    pstd::vector<TypeParam> vec;
    EXPECT_TRUE(vec.empty()) << "Vector should be empty after default construction.";
}

TYPED_TEST(vector_test, copy_constructor)
{
    auto vec{createTestValues<TypeParam>()};
    ASSERT_FALSE(vec.empty());

    auto vec_copied{vec};
    ASSERT_FALSE(vec_copied.empty());

    if constexpr (std::is_same_v<TypeParam, int>) {
        EXPECT_EQ(vec.size(), vec_copied.size());
        EXPECT_EQ(vec.front(), vec_copied.front());
        EXPECT_EQ(vec.back(), vec_copied.back());
    } else if constexpr (std::is_same_v<TypeParam, std::string>) {
        EXPECT_EQ(vec.size(), vec_copied.size());
        EXPECT_EQ(vec.front(), vec_copied.front());
        EXPECT_EQ(vec.back(), vec_copied.back());
    } else {
        EXPECT_EQ(vec.size(), vec_copied.size());
        EXPECT_EQ(vec.front(), vec_copied.front());
        EXPECT_EQ(vec.back(), vec_copied.back());
    }
}

TYPED_TEST(vector_test, initializer_list_constructor)
{
    auto vec{createTestValues<TypeParam>()};
    ASSERT_FALSE(vec.empty());

    if constexpr (std::is_same_v<TypeParam, int>) {
        EXPECT_EQ(vec.size(), 6u);
        EXPECT_EQ(vec.front(), 1);
        EXPECT_EQ(vec.back(), 6);
    } else if constexpr (std::is_same_v<TypeParam, std::string>) {
        EXPECT_EQ(vec.size(), 6u);
        EXPECT_EQ(vec.front(), "one");
        EXPECT_EQ(vec.back(), "six");
    } else {
        EXPECT_EQ(vec.size(), 3u);
        EXPECT_EQ(vec.front().x, 1.f);
        EXPECT_EQ(vec.back().s, "third");
    }
}

TYPED_TEST(vector_test, operator_bracket)
{
    auto vec{createTestValues<TypeParam>()};

    if constexpr (std::is_same_v<TypeParam, int>) {
        EXPECT_EQ(vec[0], 1u);
        EXPECT_EQ(vec[3], 4u);
    } else if constexpr (std::is_same_v<TypeParam, std::string>) {
        EXPECT_EQ(vec[0], "one");
        EXPECT_EQ(vec[3], "four");
    } else {
        EXPECT_EQ(vec[0].x, 1.f);
        EXPECT_EQ(vec[2].s, "third");
    }
    // ASSERT_EXIT(static_cast<void>(vec[1]), ::testing::KilledBySignal(SIGSEGV), ".*");
}

TEST(vector_test, insert_overload1)
{
    pstd::vector<int> vec{1, 2, 3, 4, 5};

    auto* inserted{vec.insert(vec.begin(), 100)};
    auto* inserted_end{vec.insert(vec.end(), 111)};

    EXPECT_EQ(*inserted, 100);
    EXPECT_EQ(inserted, vec.begin());
    EXPECT_EQ(*inserted_end, 111);
    EXPECT_EQ(inserted_end, vec.end() - 1);
}

TEST(vector_test, reverse)
{
    auto vec{generate_random_vector(1000, std::numeric_limits<int>::min(), std::numeric_limits<int>::max())};
    auto vec2{vec};

    std::reverse(vec.begin(), vec.end());
    std::reverse(vec2.begin(), vec2.end());
    EXPECT_EQ(vec, vec2);
}

TEST(vector_test, operator_equal)
{
    auto vec{generate_random_vector(1000, std::numeric_limits<int>::min(), std::numeric_limits<int>::max())};
    auto vec2{vec};

    EXPECT_EQ(vec, vec2);

    vec.push_back(5);
    EXPECT_NE(vec, vec2);

    vec2.push_back(6);
    EXPECT_NE(vec, vec2);
}
