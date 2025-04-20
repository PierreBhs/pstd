#include <gtest/gtest.h>

#include <pstd/ptrs/shared_ptr.hpp>

struct TestObject
{
    static int count;
    int        value;
    explicit TestObject(int v = 0) : value(v) { count++; }
    ~TestObject() { count--; }
};
int TestObject::count = 0;

TEST(SharedPtrTest, ConstructorAndDereference)
{
    pstd::shared_ptr<TestObject> ptr;
    EXPECT_EQ(ptr.get(), nullptr);
    EXPECT_EQ(ptr.use_count(), 0);

    ptr = pstd::make_shared<TestObject>(42);
    EXPECT_EQ((*ptr).value, 42);
    EXPECT_EQ(ptr->value, 42);
    EXPECT_NE(ptr.get(), nullptr);
    EXPECT_EQ(ptr.use_count(), 1);
}

TEST(SharedPtrTest, MakeSharedConstructor)
{
    struct Value
    {
        int data;
        Value(int d) : data(d) {}
    };

    auto ptr = pstd::make_shared<Value>(42);
    EXPECT_EQ(ptr->data, 42);
    EXPECT_EQ(ptr.use_count(), 1);

    auto ptr2 = ptr;
    EXPECT_EQ(ptr.use_count(), 2);
}

TEST(SharedPtrTest, ControlBlockReuse)
{
    auto ptr1 = pstd::make_shared<int>(42);
    auto ptr2 = ptr1;
    auto ptr3 = std::move(ptr1);

    EXPECT_EQ(ptr2.use_count(), 2);
    EXPECT_EQ(ptr3.use_count(), 2);
    EXPECT_EQ(ptr1.get(), nullptr);
}

TEST(SharedPtrTest, CopySemantics)
{
    auto ptr1 = pstd::make_shared<TestObject>();
    auto ptr2 = ptr1;
    EXPECT_EQ(ptr1.use_count(), 2);
    EXPECT_EQ(ptr2.use_count(), 2);
    EXPECT_EQ(ptr1.get(), ptr2.get());
}

TEST(SharedPtrTest, MoveSemantics)
{
    auto       ptr1 = pstd::make_shared<TestObject>();
    const auto addr = ptr1.get();
    auto       ptr2 = std::move(ptr1);
    EXPECT_EQ(ptr1.get(), nullptr);
    EXPECT_EQ(ptr2.get(), addr);
    EXPECT_EQ(ptr2.use_count(), 1);
}

TEST(SharedPtrTest, ResetAndDestruction)
{
    TestObject::count = 0;
    {
        auto ptr = pstd::make_shared<TestObject>();
        EXPECT_EQ(TestObject::count, 1);
        ptr.reset();
        EXPECT_EQ(TestObject::count, 0);
    }
    {
        auto ptr1 = pstd::make_shared<TestObject>();
        auto ptr2 = ptr1;
        ptr1.reset();
        EXPECT_EQ(TestObject::count, 1);
    }
    EXPECT_EQ(TestObject::count, 0);
}

TEST(SharedPtrTest, CustomDeleter)
{
    bool deleted = false;
    {
        pstd::shared_ptr<int> ptr(new int(42), [&](int* p) {
            deleted = true;
            delete p;
        });
    }
    EXPECT_TRUE(deleted);
}

TEST(SharedPtrTest, MakeSharedAllocation)
{
    auto ptr = pstd::make_shared<TestObject>(123);
    EXPECT_EQ(ptr->value, 123);
    EXPECT_EQ(ptr.use_count(), 1);
    EXPECT_EQ(TestObject::count, 1);
}

// TEST(SharedPtrTest, WeakPtrAfterReset)
// {
//     auto ptr = pstd::make_shared<TestObject>();
//     auto wptr = std::weak_ptr(ptr);
//     ptr.reset();
//     EXPECT_TRUE(wptr.expired());  // Should pass
// }
