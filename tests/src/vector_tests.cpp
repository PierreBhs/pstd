#include <gtest/gtest.h>

#include "vector/vector.hpp"

TEST(TestVector, creation)
{
    pstd::vector<int> vec{};
    ASSERT_TRUE(vec.empty());
}
