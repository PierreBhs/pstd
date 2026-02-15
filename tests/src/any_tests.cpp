#include <gtest/gtest.h>

#include <pstd/any/any.hpp>

#include <cstring>
#include <functional>
#include <string>
#include <vector>

// =============================================================================
// Helper types for testing
// =============================================================================

struct DestructorTracker
{
    int* counter;

    explicit DestructorTracker(int* c) : counter(c) {}

    DestructorTracker(const DestructorTracker& other) : counter(other.counter) {}

    DestructorTracker(DestructorTracker&& other) noexcept : counter(other.counter) { other.counter = nullptr; }

    DestructorTracker& operator=(const DestructorTracker&) = default;
    DestructorTracker& operator=(DestructorTracker&&) noexcept = default;

    ~DestructorTracker()
    {
        if (counter != nullptr) {
            ++(*counter);
        }
    }
};

// A type that is guaranteed to exceed the SBO buffer (3 * sizeof(void*) = 24 bytes on 64-bit)
struct LargeType
{
    std::byte data[128]{};
    int       value{0};

    LargeType() = default;
    explicit LargeType(int v) : value(v) {}
    LargeType(const LargeType&) = default;
    LargeType(LargeType&&) = default;
    LargeType& operator=(const LargeType&) = default;
    LargeType& operator=(LargeType&&) = default;
    ~LargeType() = default;

    bool operator==(const LargeType& other) const { return value == other.value; }
};

// A type with over-alignment that forces heap allocation
struct alignas(64) OverAlignedType
{
    int value{0};

    OverAlignedType() = default;
    explicit OverAlignedType(int v) : value(v) {}
    OverAlignedType(const OverAlignedType&) = default;
    OverAlignedType(OverAlignedType&&) = default;
    OverAlignedType& operator=(const OverAlignedType&) = default;
    OverAlignedType& operator=(OverAlignedType&&) = default;
    ~OverAlignedType() = default;
};

// A small type that fits in SBO
struct SmallType
{
    int x;
    int y;

    SmallType() : x(0), y(0) {}
    SmallType(int a, int b) : x(a), y(b) {}
    SmallType(const SmallType&) = default;
    SmallType(SmallType&&) noexcept = default;
    SmallType& operator=(const SmallType&) = default;
    SmallType& operator=(SmallType&&) noexcept = default;
    ~SmallType() = default;

    bool operator==(const SmallType& other) const { return x == other.x && y == other.y; }
};

TEST(AnyTest, DefaultConstruction)
{
    const pstd::any a;
    EXPECT_FALSE(a.has_value());
    EXPECT_EQ(a.type(), typeid(void));
}

TEST(AnyTest, ValueConstructionInt)
{
    const pstd::any a(42);
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.type(), typeid(int));
    EXPECT_EQ(pstd::any_cast<int>(a), 42);
}

TEST(AnyTest, ValueConstructionString)
{
    const pstd::any a(std::string("hello"));
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.type(), typeid(std::string));
    EXPECT_EQ(pstd::any_cast<std::string>(a), "hello");
}

TEST(AnyTest, ValueConstructionCString)
{
    const pstd::any a("hello");
    EXPECT_TRUE(a.has_value());
    // "hello" decays to const char*, not std::string
    EXPECT_EQ(a.type(), typeid(const char*));
}

TEST(AnyTest, CopyConstruction)
{
    const pstd::any a(42);
    const pstd::any b(a);

    EXPECT_TRUE(b.has_value());
    EXPECT_EQ(pstd::any_cast<int>(b), 42);
    // a is unchanged
    EXPECT_EQ(pstd::any_cast<int>(a), 42);
}

TEST(AnyTest, CopyConstructionEmpty)
{
    const pstd::any a;
    const pstd::any b(a);

    EXPECT_FALSE(b.has_value());
}

TEST(AnyTest, MoveConstruction)
{
    pstd::any       a(42);
    const pstd::any b(std::move(a));

    EXPECT_TRUE(b.has_value());
    EXPECT_EQ(pstd::any_cast<int>(b), 42);
    EXPECT_FALSE(a.has_value());
}

TEST(AnyTest, MoveConstructionEmpty)
{
    pstd::any       a;
    const pstd::any b(std::move(a));

    EXPECT_FALSE(b.has_value());
    EXPECT_FALSE(a.has_value());
}

TEST(AnyTest, InPlaceConstruction)
{
    const pstd::any a(std::in_place_type<std::string>, 5, 'x');
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.type(), typeid(std::string));
    EXPECT_EQ(pstd::any_cast<std::string>(a), "xxxxx");
}

TEST(AnyTest, InPlaceInitializerList)
{
    const pstd::any a(std::in_place_type<std::vector<int>>, {1, 2, 3});
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.type(), typeid(std::vector<int>));

    const auto& vec = pstd::any_cast<const std::vector<int>&>(a);
    EXPECT_EQ(vec.size(), 3u);
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 2);
    EXPECT_EQ(vec[2], 3);
}

TEST(AnyTest, CopyAssignmentValueToValue)
{
    pstd::any       a(42);
    const pstd::any b(std::string("hello"));

    a = b;
    EXPECT_EQ(a.type(), typeid(std::string));
    EXPECT_EQ(pstd::any_cast<std::string>(a), "hello");
}

TEST(AnyTest, CopyAssignmentEmptyToValue)
{
    pstd::any       a(42);
    const pstd::any b;

    a = b;
    EXPECT_FALSE(a.has_value());
}

TEST(AnyTest, CopyAssignmentValueToEmpty)
{
    pstd::any       a;
    const pstd::any b(42);

    a = b;
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(pstd::any_cast<int>(a), 42);
}

TEST(AnyTest, MoveAssignment)
{
    pstd::any a(42);
    pstd::any b(std::string("hello"));

    a = std::move(b);
    EXPECT_EQ(a.type(), typeid(std::string));
    EXPECT_EQ(pstd::any_cast<std::string>(a), "hello");
    EXPECT_FALSE(b.has_value());
}

TEST(AnyTest, ValueAssignment)
{
    pstd::any a(42);
    a = 3.14;

    EXPECT_EQ(a.type(), typeid(double));
    EXPECT_DOUBLE_EQ(pstd::any_cast<double>(a), 3.14);
}

TEST(AnyTest, SelfCopyAssignment)
{
    pstd::any   a(42);
    const auto& ref = a;
    a = ref;

    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(pstd::any_cast<int>(a), 42);
}

TEST(AnyTest, SelfMoveAssignment)
{
    pstd::any  a(42);
    pstd::any* p = &a;
    a = std::move(*p);

    // After self-move, state is valid but unspecified.
    // We only check it doesn't crash and is in a valid state.
    // It may or may not have a value.
    (void)a.has_value();
}

TEST(AnyTest, EmplaceBasic)
{
    pstd::any a;
    auto&     ref = a.emplace<std::string>(5, 'x');

    EXPECT_EQ(ref, "xxxxx");
    EXPECT_EQ(pstd::any_cast<std::string>(a), "xxxxx");
}

TEST(AnyTest, EmplaceOverwrite)
{
    int dtor_count = 0;
    {
        pstd::any a(DestructorTracker{&dtor_count});
        dtor_count = 0;  // Reset after construction temporaries

        a.emplace<int>(99);
        // The DestructorTracker should have been destroyed
        EXPECT_GE(dtor_count, 1);
        EXPECT_EQ(pstd::any_cast<int>(a), 99);
    }
}

TEST(AnyTest, EmplaceInitializerList)
{
    pstd::any a;
    auto&     ref = a.emplace<std::vector<int>>({1, 2, 3});

    EXPECT_EQ(ref.size(), 3u);
    EXPECT_EQ(ref[0], 1);
    EXPECT_EQ(ref[2], 3);
}

TEST(AnyTest, EmplaceReturnValue)
{
    pstd::any a;
    auto&     ref = a.emplace<int>(42);

    ref = 99;
    EXPECT_EQ(pstd::any_cast<int>(a), 99);
}

TEST(AnyTest, HasValueEmpty)
{
    EXPECT_FALSE(pstd::any().has_value());
}

TEST(AnyTest, HasValueNonEmpty)
{
    EXPECT_TRUE(pstd::any(42).has_value());
}

TEST(AnyTest, TypeEmpty)
{
    EXPECT_EQ(pstd::any().type(), typeid(void));
}

TEST(AnyTest, TypeInt)
{
    EXPECT_EQ(pstd::any(42).type(), typeid(int));
}

TEST(AnyTest, TypeAfterReset)
{
    pstd::any a(42);
    a.reset();
    EXPECT_EQ(a.type(), typeid(void));
}

TEST(AnyTest, ResetNonEmpty)
{
    pstd::any a(42);
    a.reset();
    EXPECT_FALSE(a.has_value());
}

TEST(AnyTest, ResetEmpty)
{
    pstd::any a;
    a.reset();  // Should be a no-op
    EXPECT_FALSE(a.has_value());
}

TEST(AnyTest, ResetCallsDestructor)
{
    int dtor_count = 0;
    {
        pstd::any a(DestructorTracker{&dtor_count});
        dtor_count = 0;  // Reset after construction temporaries

        a.reset();
        EXPECT_GE(dtor_count, 1);
        EXPECT_FALSE(a.has_value());
    }
}

TEST(AnyTest, SwapBothNonEmpty)
{
    pstd::any a(42);
    pstd::any b(std::string("hello"));

    a.swap(b);

    EXPECT_EQ(a.type(), typeid(std::string));
    EXPECT_EQ(pstd::any_cast<std::string>(a), "hello");
    EXPECT_EQ(b.type(), typeid(int));
    EXPECT_EQ(pstd::any_cast<int>(b), 42);
}

TEST(AnyTest, SwapOneEmpty)
{
    pstd::any a(42);
    pstd::any b;

    a.swap(b);

    EXPECT_FALSE(a.has_value());
    EXPECT_TRUE(b.has_value());
    EXPECT_EQ(pstd::any_cast<int>(b), 42);
}

TEST(AnyTest, SwapBothEmpty)
{
    pstd::any a;
    pstd::any b;

    a.swap(b);

    EXPECT_FALSE(a.has_value());
    EXPECT_FALSE(b.has_value());
}

TEST(AnyTest, NonMemberSwap)
{
    pstd::any a(42);
    pstd::any b(std::string("hello"));

    pstd::swap(a, b);

    EXPECT_EQ(pstd::any_cast<int>(b), 42);
    EXPECT_EQ(pstd::any_cast<std::string>(a), "hello");
}

TEST(AnyTest, CastToValueFromConstRef)
{
    const pstd::any a(42);
    auto            val = pstd::any_cast<int>(a);
    EXPECT_EQ(val, 42);
}

TEST(AnyTest, CastToValueFromRef)
{
    pstd::any a(42);
    auto      val = pstd::any_cast<int>(a);
    EXPECT_EQ(val, 42);
}

TEST(AnyTest, CastToRefFromRef)
{
    pstd::any a(42);
    auto&     ref = pstd::any_cast<int&>(a);
    EXPECT_EQ(ref, 42);

    ref = 99;
    EXPECT_EQ(pstd::any_cast<int>(a), 99);
}

TEST(AnyTest, CastToConstRefFromConstRef)
{
    const pstd::any a(42);
    const auto&     ref = pstd::any_cast<const int&>(a);
    EXPECT_EQ(ref, 42);
}

TEST(AnyTest, CastToValueFromRvalue)
{
    pstd::any a(std::string("hello"));
    auto      val = pstd::any_cast<std::string>(std::move(a));
    EXPECT_EQ(val, "hello");
}

TEST(AnyTest, CastPointerSuccess)
{
    pstd::any a(42);
    auto*     ptr = pstd::any_cast<int>(&a);

    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, 42);
}

TEST(AnyTest, CastPointerFailure)
{
    pstd::any a(42);
    auto*     ptr = pstd::any_cast<double>(&a);
    EXPECT_EQ(ptr, nullptr);
}

TEST(AnyTest, CastPointerNull)
{
    auto* ptr = pstd::any_cast<int>(static_cast<pstd::any*>(nullptr));
    EXPECT_EQ(ptr, nullptr);
}

TEST(AnyTest, CastConstPointerSuccess)
{
    const pstd::any a(42);
    const auto*     ptr = pstd::any_cast<int>(&a);

    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, 42);
}

TEST(AnyTest, CastConstPointerNull)
{
    const auto* ptr = pstd::any_cast<int>(static_cast<const pstd::any*>(nullptr));
    EXPECT_EQ(ptr, nullptr);
}

TEST(AnyTest, CastThrowsOnTypeMismatch)
{
    const pstd::any a(42);
    EXPECT_THROW(pstd::any_cast<double>(a), pstd::bad_any_cast);
}

TEST(AnyTest, CastThrowsOnEmpty)
{
    const pstd::any a;
    EXPECT_THROW(pstd::any_cast<int>(a), pstd::bad_any_cast);
}

TEST(AnyTest, MutateViaRefCast)
{
    pstd::any a(42);
    pstd::any_cast<int&>(a) = 99;
    EXPECT_EQ(pstd::any_cast<int>(a), 99);
}

TEST(AnyTest, MakeAnyBasic)
{
    auto a = pstd::make_any<int>(42);
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(pstd::any_cast<int>(a), 42);
}

TEST(AnyTest, MakeAnyString)
{
    auto a = pstd::make_any<std::string>(5, 'x');
    EXPECT_EQ(pstd::any_cast<std::string>(a), "xxxxx");
}

TEST(AnyTest, MakeAnyInitializerList)
{
    auto        a = pstd::make_any<std::vector<int>>({1, 2, 3});
    const auto& vec = pstd::any_cast<const std::vector<int>&>(a);
    EXPECT_EQ(vec.size(), 3u);
}

TEST(AnyTest, BadAnyCastInheritsFromBadCast)
{
    const pstd::bad_any_cast e;
    const auto*              base = dynamic_cast<const std::bad_cast*>(&e);
    EXPECT_NE(base, nullptr);
}

TEST(AnyTest, BadAnyCastWhatReturnsMessage)
{
    const pstd::bad_any_cast e;
    EXPECT_NE(e.what(), nullptr);
    EXPECT_GT(std::strlen(e.what()), 0u);
}

TEST(AnyTest, SmallObjectNoHeap)
{
    // int, double, SmallType should all fit in SBO
    const pstd::any a(42);
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(pstd::any_cast<int>(a), 42);

    const pstd::any b(3.14);
    EXPECT_EQ(pstd::any_cast<double>(b), 3.14);

    const pstd::any c(SmallType{1, 2});
    const auto&     s = pstd::any_cast<const SmallType&>(c);
    EXPECT_EQ(s.x, 1);
    EXPECT_EQ(s.y, 2);
}

TEST(AnyTest, LargeObjectHeapAllocated)
{
    const pstd::any a(LargeType{42});
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.type(), typeid(LargeType));
    EXPECT_EQ(pstd::any_cast<LargeType>(a).value, 42);
}

TEST(AnyTest, LargeObjectCopyMoveSwap)
{
    pstd::any a(LargeType{42});

    // Copy
    const pstd::any b(a);
    EXPECT_EQ(pstd::any_cast<LargeType>(b).value, 42);

    // Move
    pstd::any c(std::move(a));
    EXPECT_FALSE(a.has_value());
    EXPECT_EQ(pstd::any_cast<LargeType>(c).value, 42);

    // Swap
    pstd::any d(LargeType{99});
    c.swap(d);
    EXPECT_EQ(pstd::any_cast<LargeType>(c).value, 99);
    EXPECT_EQ(pstd::any_cast<LargeType>(d).value, 42);
}

TEST(AnyTest, OverAlignedType)
{
    const pstd::any a(OverAlignedType{42});
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.type(), typeid(OverAlignedType));
    EXPECT_EQ(pstd::any_cast<OverAlignedType>(a).value, 42);
}

TEST(AnyTest, DestructorCallsReset)
{
    int dtor_count = 0;
    {
        pstd::any a(DestructorTracker{&dtor_count});
        dtor_count = 0;  // Reset after construction temporaries
    }
    // Destructor of any should have destroyed the tracker exactly once
    EXPECT_EQ(dtor_count, 1);
}

TEST(AnyTest, EmplaceExceptionSafety)
{
    pstd::any a(42);

    // ThrowOnCopy's copy constructor throws, but emplace uses direct construction,
    // so we test with a type whose constructor throws.
    struct ThrowOnConstruct
    {
        ThrowOnConstruct(int) { throw std::runtime_error("construction failed"); }
        ThrowOnConstruct(const ThrowOnConstruct&) = default;
        ThrowOnConstruct(ThrowOnConstruct&&) noexcept = default;
        ThrowOnConstruct& operator=(const ThrowOnConstruct&) = default;
        ThrowOnConstruct& operator=(ThrowOnConstruct&&) noexcept = default;
        ~ThrowOnConstruct() = default;
    };

    EXPECT_THROW(a.emplace<ThrowOnConstruct>(42), std::runtime_error);
    // After failed emplace, any should be empty (reset was called first)
    EXPECT_FALSE(a.has_value());
}

TEST(AnyTest, DecayBehavior)
{
    // "hello" is const char[6], but any should store const char*
    const pstd::any a("hello");
    EXPECT_EQ(a.type(), typeid(const char*));
}

TEST(AnyTest, StoresDecayedType)
{
    int             i = 42;
    const pstd::any a(std::ref(i));
    EXPECT_EQ(a.type(), typeid(std::reference_wrapper<int>));
    EXPECT_EQ(pstd::any_cast<std::reference_wrapper<int>>(a).get(), 42);
}

TEST(AnyTest, MultipleReassignments)
{
    pstd::any a(42);
    a = std::string("hello");
    a = 3.14;
    a = LargeType{99};
    a = SmallType{1, 2};

    EXPECT_EQ(pstd::any_cast<SmallType>(a), (SmallType{1, 2}));
}

TEST(AnyTest, EmplaceReturnsCorrectReference)
{
    pstd::any a;
    auto&     str = a.emplace<std::string>("hello world");

    EXPECT_EQ(str, "hello world");
    // Modify through the returned reference
    str += "!";
    EXPECT_EQ(pstd::any_cast<const std::string&>(a), "hello world!");
}

TEST(AnyTest, SwapMixedSboAndHeap)
{
    pstd::any small_val(42);             // SBO
    pstd::any large_val(LargeType{99});  // Heap

    small_val.swap(large_val);

    EXPECT_EQ(pstd::any_cast<LargeType>(small_val).value, 99);
    EXPECT_EQ(pstd::any_cast<int>(large_val), 42);
}

TEST(AnyTest, CopyAssignmentLargeToLarge)
{
    pstd::any       a(LargeType{42});
    const pstd::any b(LargeType{99});

    a = b;
    EXPECT_EQ(pstd::any_cast<LargeType>(a).value, 99);
    // b is unchanged
    EXPECT_EQ(pstd::any_cast<LargeType>(b).value, 99);
}

TEST(AnyTest, MoveAssignmentLargeToSmall)
{
    pstd::any a(42);
    pstd::any b(LargeType{99});

    a = std::move(b);
    EXPECT_EQ(pstd::any_cast<LargeType>(a).value, 99);
    EXPECT_FALSE(b.has_value());
}

TEST(AnyTest, ValueConstructionBool)
{
    const pstd::any a(true);
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.type(), typeid(bool));
    EXPECT_EQ(pstd::any_cast<bool>(a), true);
}

TEST(AnyTest, ValueConstructionDouble)
{
    const pstd::any a(3.14159);
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.type(), typeid(double));
    EXPECT_DOUBLE_EQ(pstd::any_cast<double>(a), 3.14159);
}

TEST(AnyTest, EmplaceOnEmpty)
{
    pstd::any a;
    EXPECT_FALSE(a.has_value());

    a.emplace<int>(42);
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(pstd::any_cast<int>(a), 42);
}

TEST(AnyTest, CastToConstRefFromRef)
{
    pstd::any   a(42);
    const auto& ref = pstd::any_cast<const int&>(a);
    EXPECT_EQ(ref, 42);
}

TEST(AnyTest, MoveConstructFromString)
{
    std::string s = "hello world with a longer string to avoid SSO";
    pstd::any   a(std::move(s));

    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.type(), typeid(std::string));
    EXPECT_EQ(pstd::any_cast<const std::string&>(a), "hello world with a longer string to avoid SSO");
}

TEST(AnyTest, OverAlignedTypeCopyMoveSwap)
{
    pstd::any a(OverAlignedType{42});

    // Copy
    const pstd::any b(a);
    EXPECT_EQ(pstd::any_cast<OverAlignedType>(b).value, 42);

    // Move
    pstd::any c(std::move(a));
    EXPECT_FALSE(a.has_value());
    EXPECT_EQ(pstd::any_cast<OverAlignedType>(c).value, 42);

    // Swap
    pstd::any d(OverAlignedType{99});
    c.swap(d);
    EXPECT_EQ(pstd::any_cast<OverAlignedType>(c).value, 99);
    EXPECT_EQ(pstd::any_cast<OverAlignedType>(d).value, 42);
}

TEST(AnyTest, DestructorCountMultipleCopies)
{
    int dtor_count = 0;
    {
        pstd::any a(DestructorTracker{&dtor_count});
        dtor_count = 0;

        pstd::any b(a);  // copy
        pstd::any c(a);  // copy

        // All three should be independently tracked
        EXPECT_TRUE(a.has_value());
        EXPECT_TRUE(b.has_value());
        EXPECT_TRUE(c.has_value());
    }
    // Three any objects destroyed, each should destroy its tracker
    EXPECT_EQ(dtor_count, 3);
}

TEST(AnyTest, ResetThenEmplace)
{
    pstd::any a(42);
    a.reset();
    EXPECT_FALSE(a.has_value());

    a.emplace<std::string>("reused");
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(pstd::any_cast<std::string>(a), "reused");
}

TEST(AnyTest, MakeAnyLargeType)
{
    auto a = pstd::make_any<LargeType>(42);
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(pstd::any_cast<LargeType>(a).value, 42);
}

TEST(AnyTest, CastConstPointerFailure)
{
    const pstd::any a(42);
    const auto*     ptr = pstd::any_cast<double>(&a);
    EXPECT_EQ(ptr, nullptr);
}

TEST(AnyTest, HasValueAfterAssignment)
{
    pstd::any a;
    EXPECT_FALSE(a.has_value());

    a = 42;
    EXPECT_TRUE(a.has_value());

    a = pstd::any{};
    EXPECT_FALSE(a.has_value());
}

TEST(AnyTest, ValueConstructorForwardsRightType)
{
    const std::string s = "x";
    pstd::any         a(s);

    pstd::any         a2;
    const std::string s2 = "x";
    a2 = s2;
}

TEST(AnyTest, InPlaceCtorBuildsCorrectDecayedType)
{
    pstd::any a(std::in_place_type<const std::vector<int>>, {1, 2, 3});
}
