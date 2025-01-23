#include "utilities.hpp"
#include "vector/vector.hpp"

template <typename>
struct TD;

#include <vector>

int main()
{
    pstd::vector<int> vec{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    pstd::vector<int> vec2{55, 55, 55, 55, 55, 55, 5, 5, 5};

    std::println("{} {} | {} {}", vec.size(), vec.capacity(), vec2.size(), vec2.capacity());
    vec2 = std::move(vec);

    std::println("{} {} | {} {}", vec.size(), vec.capacity(), vec2.size(), vec2.capacity());
    std::println("{}", vec2[11]);

    return 0;
}

// auto vec{
//     pstd::utilities::generate_random_vector(10, std::numeric_limits<int>::min(), std::numeric_limits<int>::max())};
// auto vec2{std::move(vec)};
// pstd::vector<int> vec{1, 2, 3, 4, 5, 6, 7, 8};

// const int& pos2{vec.at(2)};
// std::println("{} {}", vec.at(4), pos2);

// pstd::utilities::print_vector<>(vec);
