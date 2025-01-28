#include <gtest/gtest.h>
#include "unique_ptr/unique_ptr.hpp"

struct DeletionTracker
{
    static int count;
    void       operator()(int* p) const noexcept
    {
        delete p;
        ++count;
    }
};
int DeletionTracker::count = 0;

TEST(UniquePtrTest, BasicFunctionality)
{
    {
        pstd::unique_ptr<int> ptr(new int(42));
        EXPECT_EQ(*ptr, 42);
        EXPECT_NE(ptr.get(), nullptr);
    }  // Should not leak
}

TEST(UniquePtrTest, MoveSemantics)
{
    pstd::unique_ptr<int> ptr1(new int(42));
    pstd::unique_ptr<int> ptr2 = std::move(ptr1);
    EXPECT_EQ(ptr1.get(), nullptr);
    EXPECT_EQ(*ptr2, 42);
}

TEST(UniquePtrTest, CustomDeleter)
{
    DeletionTracker::count = 0;
    {
        pstd::unique_ptr<int, DeletionTracker> ptr(new int(42));
        EXPECT_EQ(DeletionTracker::count, 0);
    }
    EXPECT_EQ(DeletionTracker::count, 1);
}

// TEST(UniquePtrTest, ArraySpecialization)
// {
//     {
//         pstd::unique_ptr<int[]> ptr(new int[5]);
//         ptr[0] = 1;
//         ptr[1] = 2;
//         EXPECT_EQ(ptr[0], 1);
//         EXPECT_EQ(ptr[1], 2);
//     }  // Should call delete[]
// }

TEST(UniquePtrTest, ReleaseReset)
{
    pstd::unique_ptr<int> ptr(new int(42));
    int*                  raw = ptr.release();
    EXPECT_EQ(ptr.get(), nullptr);
    delete raw;
}

TEST(UniquePtrTest, MoveAssignment)
{
    pstd::unique_ptr<int> ptr1(new int(42));
    pstd::unique_ptr<int> ptr2;
    ptr2 = std::move(ptr1);
    EXPECT_EQ(ptr1.get(), nullptr);
    EXPECT_EQ(*ptr2, 42);
}

TEST(UniquePtrTest, NullptrAssignment)
{
    pstd::unique_ptr<int> ptr(new int(42));
    ptr = nullptr;
    EXPECT_EQ(ptr.get(), nullptr);
}

// TEST(UniquePtrTest, ConversionConstructor)
// {
//     pstd::unique_ptr<Derived> derived_ptr(new Derived);
//     pstd::unique_ptr<Base>    base_ptr(std::move(derived_ptr));
//     EXPECT_NE(base_ptr.get(), nullptr);
// }

TEST(UniquePtrTest, BoolConversion)
{
    pstd::unique_ptr<int> valid_ptr(new int(42));
    pstd::unique_ptr<int> empty_ptr;
    EXPECT_TRUE(valid_ptr);
    EXPECT_FALSE(empty_ptr);
}

// Edge Cases
TEST(UniquePtrTest, NullptrConstruction)
{
    pstd::unique_ptr<int> ptr(nullptr);
    EXPECT_EQ(ptr.get(), nullptr);
}

TEST(UniquePtrTest, StatefulDeleter)
{
    struct StatefulDeleter
    {
        int* counter;
        void operator()(int* p) noexcept
        {
            delete p;
            ++(*counter);
        }
    };

    int counter = 0;
    {
        pstd::unique_ptr<int, StatefulDeleter> ptr(new int(42), StatefulDeleter{&counter});
        EXPECT_EQ(counter, 0);
    }
    EXPECT_EQ(counter, 1);
}

// TEST(UniquePtrTest, ArrayBoundaryCheck)
// {
//     pstd::unique_ptr<int[]> ptr(new int[3]{1, 2, 3});
//     EXPECT_EQ(ptr[0], 1);
//     EXPECT_EQ(ptr[2], 3);
// }
