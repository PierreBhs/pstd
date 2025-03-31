#include "gtest/gtest.h"
#include "heap/binary_heap.hpp"  // Adjust path as needed

#include <algorithm>  // std::is_sorted
#include <compare>    // std::strong_ordering
#include <string>
#include <vector>

struct Point
{
    int x, y;

    bool operator<(const Point& other) const
    {
        if (x != other.x)
            return x < other.x;
        return y < other.y;
    }
    bool                 operator==(const Point& other) const { return x == other.x && y == other.y; }
    std::strong_ordering operator<=>(const Point& other) const = default;
};

struct ComparePointYDescending
{
    bool operator()(const Point& a, const Point& b) const
    {
        return a.y < b.y;  // Less y means lower priority -> max heap by Y
    }
};

class BinaryHeapTest : public ::testing::Test
{
protected:
    template <typename Heap>
    std::vector<typename Heap::value_type> pop_all(Heap& heap)
    {
        std::vector<typename Heap::value_type> result;
        while (!heap.empty()) {
// Need to handle potential exception from top() if implementation throws
#ifdef NDEBUG  // Only catch in release if asserts are disabled and exceptions used
            try {
                result.push_back(heap.top());
                heap.pop();
            } catch (const std::out_of_range& e) {
                // This shouldn't happen if !heap.empty() is checked, but good practice
                ADD_FAILURE() << "Unexpected exception during pop_all: " << e.what();
                break;
            }
#else  // In debug, rely on assert or just call directly
            result.push_back(heap.top());  // Assumes top() doesn't throw or assert handled
            heap.pop();                    // Assumes pop() doesn't throw or assert handled
#endif
        }
        return result;
    }
};

TEST(BinaryHeapBasic, DefaultConstructorIsEmpty)
{
    pstd::binary_heap<int> pq;
    EXPECT_TRUE(pq.empty());
    EXPECT_EQ(pq.size(), 0);
    // top() and pop() on empty should assert or throw (depending on build/implementation)
    // GTest cannot easily check asserts without specific setup (EXPECT_DEBUG_DEATH)
    // If using exceptions:
    // EXPECT_THROW(pq.top(), std::out_of_range);
    // EXPECT_THROW(pq.pop(), std::out_of_range);
}

TEST(BinaryHeapBasic, PushIncreasesSize)
{
    pstd::binary_heap<int> pq;
    EXPECT_EQ(pq.size(), 0);
    pq.push(10);
    EXPECT_EQ(pq.size(), 1);
    EXPECT_FALSE(pq.empty());
    pq.push(20);
    EXPECT_EQ(pq.size(), 2);
}

TEST(BinaryHeapBasic, EmplaceIncreasesSize)
{
    pstd::binary_heap<std::pair<int, std::string>> pq;
    EXPECT_EQ(pq.size(), 0);
    pq.emplace(10, "ten");
    EXPECT_EQ(pq.size(), 1);
    EXPECT_FALSE(pq.empty());
    pq.emplace(20, "twenty");
    EXPECT_EQ(pq.size(), 2);
}

// --- Max Heap Tests (Default Comparator: std::less) ---

TEST_F(BinaryHeapTest, MaxHeapPushAndTop)
{
    pstd::binary_heap<int> pq;
    pq.push(10);
    EXPECT_EQ(pq.top(), 10);
    pq.push(5);
    EXPECT_EQ(pq.top(), 10);  // Max remains 10
    pq.push(20);
    EXPECT_EQ(pq.top(), 20);  // New max is 20
    pq.push(15);
    EXPECT_EQ(pq.top(), 20);  // Max remains 20
    EXPECT_EQ(pq.size(), 4);
}

TEST_F(BinaryHeapTest, MaxHeapPopOrder)
{
    pstd::binary_heap<int> pq;
    std::vector<int>       data = {30, 10, 50, 20, 40};
    for (int x : data) {
        pq.push(x);
    }

    EXPECT_EQ(pq.size(), 5);

    EXPECT_EQ(pq.top(), 50);
    pq.pop();
    EXPECT_EQ(pq.size(), 4);
    EXPECT_EQ(pq.top(), 40);
    pq.pop();
    EXPECT_EQ(pq.size(), 3);
    EXPECT_EQ(pq.top(), 30);
    pq.pop();
    EXPECT_EQ(pq.size(), 2);
    EXPECT_EQ(pq.top(), 20);
    pq.pop();
    EXPECT_EQ(pq.size(), 1);
    EXPECT_EQ(pq.top(), 10);
    pq.pop();
    EXPECT_EQ(pq.size(), 0);
    EXPECT_TRUE(pq.empty());
}

TEST_F(BinaryHeapTest, MaxHeapPopAllIsSortedDescending)
{
    pstd::binary_heap<int> pq;
    std::vector<int>       data = {3, 1, 4, 1, 5, 9, 2, 6};
    std::vector<int>       sorted_data = data;
    std::sort(sorted_data.rbegin(), sorted_data.rend());  // Sort descending

    for (int x : data) {
        pq.push(x);
    }

    auto popped_elements = pop_all(pq);
    EXPECT_EQ(popped_elements, sorted_data);
    EXPECT_TRUE(pq.empty());
}

TEST_F(BinaryHeapTest, MaxHeapDuplicateElements)
{
    pstd::binary_heap<int> pq;
    std::vector<int>       data = {10, 20, 5, 20, 15, 10, 5};
    for (int x : data) {
        pq.push(x);
    }
    EXPECT_EQ(pq.size(), 7);
    EXPECT_EQ(pq.top(), 20);
    pq.pop();  // First 20
    EXPECT_EQ(pq.top(), 20);
    pq.pop();  // Second 20
    EXPECT_EQ(pq.top(), 15);
    pq.pop();
    EXPECT_EQ(pq.top(), 10);
    pq.pop();  // First 10
    EXPECT_EQ(pq.top(), 10);
    pq.pop();  // Second 10
    EXPECT_EQ(pq.top(), 5);
    pq.pop();  // First 5
    EXPECT_EQ(pq.top(), 5);
    pq.pop();  // Second 5
    EXPECT_TRUE(pq.empty());
}

// --- Min Heap Tests (Comparator: std::greater) ---

TEST_F(BinaryHeapTest, MinHeapPushAndTop)
{
    pstd::binary_heap<int, std::vector<int>, std::greater<int>> pq;
    pq.push(10);
    EXPECT_EQ(pq.top(), 10);
    pq.push(5);
    EXPECT_EQ(pq.top(), 5);  // Min is 5
    pq.push(20);
    EXPECT_EQ(pq.top(), 5);  // Min remains 5
    pq.push(2);
    EXPECT_EQ(pq.top(), 2);  // New min is 2
    EXPECT_EQ(pq.size(), 4);
}

TEST_F(BinaryHeapTest, MinHeapPopOrder)
{
    pstd::binary_heap<int, std::vector<int>, std::greater<int>> pq;
    std::vector<int>                                            data = {30, 10, 50, 20, 40};
    for (int x : data) {
        pq.push(x);
    }

    EXPECT_EQ(pq.size(), 5);

    EXPECT_EQ(pq.top(), 10);
    pq.pop();
    EXPECT_EQ(pq.size(), 4);
    EXPECT_EQ(pq.top(), 20);
    pq.pop();
    EXPECT_EQ(pq.size(), 3);
    EXPECT_EQ(pq.top(), 30);
    pq.pop();
    EXPECT_EQ(pq.size(), 2);
    EXPECT_EQ(pq.top(), 40);
    pq.pop();
    EXPECT_EQ(pq.size(), 1);
    EXPECT_EQ(pq.top(), 50);
    pq.pop();
    EXPECT_EQ(pq.size(), 0);
    EXPECT_TRUE(pq.empty());
}

TEST_F(BinaryHeapTest, MinHeapPopAllIsSortedAscending)
{
    pstd::binary_heap<int, std::vector<int>, std::greater<int>> pq;
    std::vector<int>                                            data = {3, 1, 4, 1, 5, 9, 2, 6};
    std::vector<int>                                            sorted_data = data;
    std::sort(sorted_data.begin(), sorted_data.end());  // Sort ascending

    for (int x : data) {
        pq.push(x);
    }

    auto popped_elements = pop_all(pq);
    EXPECT_EQ(popped_elements, sorted_data);
    EXPECT_TRUE(pq.empty());
}

// --- Constructor Tests ---

TEST_F(BinaryHeapTest, ConstructFromIteratorsMaxHeap)
{
    std::vector<int>       data = {3, 1, 4, 1, 5, 9, 2, 6};
    pstd::binary_heap<int> pq(data.begin(), data.end());

    EXPECT_EQ(pq.size(), data.size());
    EXPECT_EQ(pq.top(), 9);

    std::vector<int> sorted_data = data;
    std::sort(sorted_data.rbegin(), sorted_data.rend());  // Sort descending
    auto popped_elements = pop_all(pq);
    EXPECT_EQ(popped_elements, sorted_data);
    EXPECT_TRUE(pq.empty());
}

TEST_F(BinaryHeapTest, ConstructFromIteratorsMinHeap)
{
    std::vector<int>                                            data = {3, 1, 4, 1, 5, 9, 2, 6};
    pstd::binary_heap<int, std::vector<int>, std::greater<int>> pq(data.begin(), data.end(), std::greater<int>{});

    EXPECT_EQ(pq.size(), data.size());
    EXPECT_EQ(pq.top(), 1);

    std::vector<int> sorted_data = data;
    std::sort(sorted_data.begin(), sorted_data.end());  // Sort ascending
    auto popped_elements = pop_all(pq);
    EXPECT_EQ(popped_elements, sorted_data);
    EXPECT_TRUE(pq.empty());
}

TEST_F(BinaryHeapTest, ConstructFromContainerMaxHeap)
{
    std::vector<int>       data = {3, 1, 4, 1, 5, 9, 2, 6};
    pstd::binary_heap<int> pq({}, data);  // Use default compare, pass container

    EXPECT_EQ(pq.size(), data.size());
    EXPECT_EQ(pq.top(), 9);
}

TEST_F(BinaryHeapTest, ConstructFromMovedContainerMaxHeap)
{
    std::vector<int>       data = {3, 1, 4, 1, 5, 9, 2, 6};
    std::vector<int>       data_copy = data;         // Keep a copy for verification
    pstd::binary_heap<int> pq({}, std::move(data));  // Use default compare, move container

    EXPECT_EQ(pq.size(), data_copy.size());
    EXPECT_TRUE(data.empty() || data.capacity() == 0);  // Moved-from vector state is valid but unspecified, often empty
    EXPECT_EQ(pq.top(), 9);

    std::sort(data_copy.rbegin(), data_copy.rend());  // Sort descending
    auto popped_elements = pop_all(pq);
    EXPECT_EQ(popped_elements, data_copy);
    EXPECT_TRUE(pq.empty());
}

// --- Swap Test ---

TEST_F(BinaryHeapTest, SwapFunction)
{
    pstd::binary_heap<int> pq_max;
    pq_max.push(10);
    pq_max.push(30);
    pq_max.push(20);  // Top: 30, Size: 3

    pstd::binary_heap<int, std::vector<int>, std::greater<int>> pq_min;
    pq_min.push(5);
    pq_min.push(1);
    pq_min.push(3);
    pq_min.push(8);  // Top: 1, Size: 4

    // Need adapter or temporary for swapping heaps with different types (comparator)
    // Let's swap two max heaps for simplicity first
    pstd::binary_heap<int> pq_max2;
    pq_max2.push(100);
    pq_max2.push(200);  // Top: 200, Size: 2

    // Check initial states
    EXPECT_EQ(pq_max.size(), 3);
    EXPECT_EQ(pq_max.top(), 30);
    EXPECT_EQ(pq_max2.size(), 2);
    EXPECT_EQ(pq_max2.top(), 200);

    swap(pq_max, pq_max2);

    // Check swapped states
    EXPECT_EQ(pq_max.size(), 2);
    EXPECT_EQ(pq_max.top(), 200);
    EXPECT_EQ(pq_max2.size(), 3);
    EXPECT_EQ(pq_max2.top(), 30);

    // Pop from swapped heaps to verify further
    pq_max.pop();
    EXPECT_EQ(pq_max.top(), 100);
    pq_max2.pop();
    EXPECT_EQ(pq_max2.top(), 20);

    // Swap with empty
    pstd::binary_heap<int> pq_empty;
    swap(pq_max, pq_empty);
    EXPECT_TRUE(pq_max.empty());
    EXPECT_EQ(pq_empty.size(), 1);  // Had 100 left
    EXPECT_EQ(pq_empty.top(), 100);
}

// --- Custom Type and Comparator Test ---

TEST_F(BinaryHeapTest, CustomTypeWithDefaultComparator)
{
    pstd::binary_heap<Point> pq;  // Uses Point::operator< -> max heap by X then Y
    Point                    p1{1, 10}, p2{5, 5}, p3{2, 20}, p4{5, 1};
    pq.push(p1);
    pq.push(p2);
    pq.push(p3);
    pq.push(p4);

    EXPECT_EQ(pq.top(), p2);
    pq.pop();  // {5, 5} (max X, max Y among X=5)
    EXPECT_EQ(pq.top(), p4);
    pq.pop();  // {5, 1}
    EXPECT_EQ(pq.top(), p3);
    pq.pop();  // {2, 20}
    EXPECT_EQ(pq.top(), p1);
    pq.pop();  // {1, 10}
    EXPECT_TRUE(pq.empty());
}

TEST_F(BinaryHeapTest, CustomTypeWithCustomComparator)
{
    // Max heap based on Y coordinate
    pstd::binary_heap<Point, std::vector<Point>, ComparePointYDescending> pq;
    Point                                                                 p1{1, 10}, p2{5, 5}, p3{2, 20}, p4{5, 1};
    pq.push(p1);
    pq.push(p2);
    pq.push(p3);
    pq.push(p4);

    EXPECT_EQ(pq.top(), p3);
    pq.pop();  // {2, 20} (max Y)
    EXPECT_EQ(pq.top(), p1);
    pq.pop();  // {1, 10}
    EXPECT_EQ(pq.top(), p2);
    pq.pop();  // {5, 5}
    EXPECT_EQ(pq.top(), p4);
    pq.pop();  // {5, 1}
    EXPECT_TRUE(pq.empty());
}

// --- String Data Test ---
TEST_F(BinaryHeapTest, StringDataMaxHeap)
{
    pstd::binary_heap<std::string> pq;
    pq.push("apple");
    pq.push("banana");
    pq.push("orange");
    pq.push("grape");
    pq.push("zz");  // Should be top

    EXPECT_EQ(pq.top(), "zz");
    pq.pop();
    EXPECT_EQ(pq.top(), "orange");
    pq.pop();
    EXPECT_EQ(pq.top(), "grape");
    pq.pop();
    EXPECT_EQ(pq.top(), "banana");
    pq.pop();
    EXPECT_EQ(pq.top(), "apple");
    pq.pop();
    EXPECT_TRUE(pq.empty());
}

// --- Move Semantics Test ---
TEST_F(BinaryHeapTest, PushRValue)
{
    pstd::binary_heap<std::string> pq;
    std::string                    s1 = "hello";
    std::string                    s2 = "world";

    pq.push(std::move(s1));
    pq.push(std::move(s2));

    // Moved-from strings are in a valid but unspecified state (often empty)
    EXPECT_TRUE(s1.empty() || s1.capacity() == 0);  // Check if moved from
    EXPECT_TRUE(s2.empty() || s2.capacity() == 0);

    EXPECT_EQ(pq.size(), 2);
    EXPECT_EQ(pq.top(), "world");  // Lexicographically greater
    pq.pop();
    EXPECT_EQ(pq.top(), "hello");
}
