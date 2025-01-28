#include "unique_ptr/unique_ptr.hpp"
#include "utilities.hpp"
#include "vector/vector.hpp"

template <typename>
struct TD;

#include <vector>

int main()
{
    pstd::vector<int> vec{};

    vec.emplace(vec.begin(), 35);
    vec.emplace(vec.begin(), 25);
    vec.emplace(vec.begin(), 135);

    std::println("{} {} {}", vec[0], vec[1], vec[2]);

    pstd::vector<pstd::unique_ptr<int>> vec_ptr;

    for (auto i{0ul}; i < 10ul; ++i) {
        vec_ptr.emplace_back(pstd::make_unique<int>(i));
        std::println("{}", *vec_ptr.back());
    }

    return 0;
}

// auto vec{
//     pstd::utilities::generate_random_vector(10, std::numeric_limits<int>::min(), std::numeric_limits<int>::max())};
// auto vec2{std::move(vec)};
// pstd::vector<int> vec{1, 2, 3, 4, 5, 6, 7, 8};

// const int& pos2{vec.at(2)};
// std::println("{} {}", vec.at(4), pos2);

// pstd::utilities::print_vector<>(vec);
