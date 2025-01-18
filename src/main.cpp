#include "utilities.hpp"
#include "vector/vector.hpp"

#include <vector>

int main()
{
    // pstd::vector<pstd::utilities::Lifetime> vec(5);

    // std::print("emplace_back #1\n");
    // vec.emplace_back();
    // std::print("emplace_back #2\n");
    // vec.emplace_back();
    // std::print("emplace_back #3\n");
    // vec.emplace_back();
    // std::print("emplace_back #4\n");
    // vec.push_back({});

    auto vec{
        pstd::utilities::generate_random_vector(10, std::numeric_limits<int>::min(), std::numeric_limits<int>::max())};
    auto vec2{vec};

    vec2.clear();

    int a{420};
    vec.insert(vec.begin() + 5, a);
    vec2.insert(vec2.begin() + 5, 1111);

    pstd::utilities::print_vector<>(vec);
    pstd::utilities::print_vector<>(vec2);

    return 0;
}
