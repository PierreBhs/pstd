#include "ptrs/shared_ptr.hpp"
#include "ptrs/unique_ptr.hpp"
#include "utilities.hpp"
#include "vector/vector.hpp"

#include <iostream>

struct S
{
    S() { std::cout << "S::S()\n"; }
    ~S() { std::cout << "S::~S()\n"; }
    struct Deleter
    {
        void operator()(S* s) const
        {
            std::cout << "S::Deleter()\n";
            delete s;
        }
    };
};

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

    auto sp = pstd::shared_ptr<S>{new S, S::Deleter{}};

    auto use_count = [&sp](char c) {
        std::cout << c << ") use_count(): " << sp.use_count() << '\n';
    };

    use_count('A');
    {
        auto sp2 = sp;
        use_count('B');
        {
            auto sp3 = sp;
            use_count('C');
        }
        use_count('D');
    }
    use_count('E');
    sp.reset();
    use_count('F');  // would print "F) use_count(): 0"
    return 0;
}

// auto vec{
//     pstd::utilities::generate_random_vector(10, std::numeric_limits<int>::min(), std::numeric_limits<int>::max())};
// auto vec2{std::move(vec)};
// pstd::vector<int> vec{1, 2, 3, 4, 5, 6, 7, 8};

// const int& pos2{vec.at(2)};
// std::println("{} {}", vec.at(4), pos2);

// pstd::utilities::print_vector<>(vec);
