#include "vector/vector.hpp"

#include <print>

namespace pstd::utilities {

template <typename T>
void print_vector(const vector<T>& vec)
{
    for (auto i{0ul}; i < vec.size(); ++i) {
        std::print("{0} ", vec[i]);
    }

    // Apple Clang still doesnt have println()...
    std::print("\n");
}
}  // namespace pstd::utilities
