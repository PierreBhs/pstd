#include <gtest/gtest.h>

#include <pstd/iterator/iterator.hpp>
#include <pstd/vector/vector.hpp>

TEST(IteratorTest, BasicFunctionality)
{
    pstd::vector<int>   vec{1, 2, 3};
    pstd::iterator<int> begin(vec.data());
    pstd::iterator<int> end(vec.data() + vec.size());

    EXPECT_EQ(*begin, 1);
    ++begin;
    EXPECT_EQ(*begin, 2);
    begin++;
    EXPECT_EQ(*begin, 3);

    EXPECT_NE(begin, end);
    ++begin;
    EXPECT_EQ(begin, end);
}

TEST(IteratorTest, ConceptCheck)
{
    static_assert(std::forward_iterator<pstd::iterator<int>>);
}

// TEST(RandomIteratorTest, CoreOperations)
// {
//     pstd::vector<int>          vec{1, 2, 3, 4};
//     pstd::random_iterator<int> begin(vec.data());
//     pstd::random_iterator<int> end(vec.data() + vec.size());

//     EXPECT_EQ(*begin, 1);
//     EXPECT_EQ(begin[2], 3);

//     ++begin;
//     EXPECT_EQ(*begin, 2);
//     begin++;
//     EXPECT_EQ(*begin, 3);

//     --end;
//     EXPECT_EQ(*end, 4);
//     end--;
//     EXPECT_EQ(*end, 3);

//     auto it = begin + 2;
//     EXPECT_EQ(*it, 4);
//     it = it - 1;
//     EXPECT_EQ(*it, 3);

//     EXPECT_LT(begin, end);
//     EXPECT_GT(end, begin);
//     EXPECT_LE(begin, begin);
//     EXPECT_GE(end, end);
// }

TEST(RandomIteratorTest, ConceptCheck)
{
    static_assert(std::random_access_iterator<pstd::random_iterator<int>>);
    static_assert(std::sentinel_for<pstd::random_iterator<int>, pstd::random_iterator<int>>);
}
