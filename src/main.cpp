#include "utilities.hpp"
#include "vector/vector.hpp"

int main()
{
    pstd::vector<int> vec{42, 12, 13};

    vec.push_back(123);
    vec.push_back(123);
    vec.push_back(123);
    vec.push_back(123);
    vec.push_back(123);
    vec.push_back(123);
    vec.push_back(123);
    vec.push_back(123);
    pstd::utilities::print_vector<>(vec);
    return 0;
}
