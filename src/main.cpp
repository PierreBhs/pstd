#include "utilities.hpp"
#include "vector/vector.hpp"

struct Data
{
    int         data{0};
    std::string str{"str"};
};

int main()
{
    pstd::vector<Data> vec{{}, {}, {}};

    vec.push_back({});
    vec.push_back({});
    vec.push_back({});
    vec.push_back({});
    vec.push_back({});
    vec.push_back({});
    vec.push_back({});
    vec.push_back({});
    // pstd::utilities::print_vector<>(vec);
    return 0;
}
