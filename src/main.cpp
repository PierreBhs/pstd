#include "utilities.hpp"
#include "vector/vector.hpp"

int main()
{
    // auto vec{
    //     pstd::utilities::generate_random_vector(10, std::numeric_limits<int>::min(), std::numeric_limits<int>::max())};
    // auto vec2{std::move(vec)};
    pstd::vector<int> vec{1, 2, 3, 4, 5, 6, 7, 8};

    vec.pop_back();
    vec.pop_back();
    vec.pop_back();
    vec.pop_back();
    pstd::utilities::print_vector<>(vec);

    return 0;
}
