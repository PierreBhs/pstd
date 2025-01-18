#include "utilities.hpp"
#include "vector/vector.hpp"

#include <vector>

int main()
{
    // pstd::vector<pstd::utilities::Lifetime> vec{};
    pstd::vector<int> vec{1, 2, 3, 4, 5};
    // std::print("emplace_back #1\n");
    // vec.emplace_back();
    // std::print("emplace_back #2\n");
    // vec.emplace_back();
    // std::print("emplace_back #3\n");
    // vec.emplace_back();
    // std::print("emplace_back #4\n");
    // vec.push_back({});

    // vec.clear();

    // vec.push_back({});

    // std::print("PUSH_BACK\n");
    // vec.push_back(3);
    // vec.emplace_back(2);

    // vec.insert(vec.begin(), 111);
    auto* it = vec.insert(vec.begin(), 112);
    // vec.insert(vec.begin() + 4, 113);
    // vec.insert(vec.end(), 114);

    // std::print("PRINTING FRONT AND BACK\n");
    // std::print("{0} {1}\n", vec.front(), vec.back());

    std::println("{} {}", (std::size_t)vec.begin(), (std::size_t)it);
    pstd::utilities::print_vector<>(vec);
    std::print("vec[0] = {} vec[100] = {}\n", vec[0], vec[-1]);
    return 0;
}
