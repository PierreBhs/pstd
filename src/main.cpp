#include "utilities.hpp"
#include "vector/vector.hpp"

#include <vector>

int main()
{
    std::print("CREATE VEC\n");
    pstd::vector<pstd::utilities::Lifetime> vec{};
    std::print("PUSH_BACK #1\n");
    vec.push_back({});
    std::print("PUSH_BACK #2\n");
    vec.push_back({});
    std::print("PUSH_BACK #3\n");
    vec.push_back({});
    std::print("PUSH_BACK #4\n");
    pstd::utilities::Lifetime lf{};

    vec.push_back(lf);

    // pstd::utilities::print_vector<>(vec);
    std::print("ËXITING\n");
    return 0;
}
