#include <iostream>

#include <pstd/ptrs/unique_ptr.hpp>
#include <pstd/vector/vector.hpp>

int main()
{
    pstd::vector<int> nums{1, 2, 3};
    nums.push_back(4);

    std::cout << "Vector contents: ";
    for (const auto& n : nums) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    auto ptr = pstd::make_unique<double>(3.14159);
    std::cout << "Unique ptr value: " << *ptr << "\n";

    return 0;
}
