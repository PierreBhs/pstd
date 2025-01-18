#include "vector/vector.hpp"

#include <print>
#include <random>

namespace pstd::utilities {

struct Lifetime
{

    Lifetime() : id(++id_counter)
    {
        ++count;
        std::print("Default constructor called, id = {}, count = {}\n", id, count);
    }

    Lifetime(const Lifetime& other) : id(++id_counter), text{other.text}
    {
        ++count;
        std::print("Copy constructor called, id = {}, count = {}\n", id, count);
    }

    Lifetime(Lifetime&& other) noexcept : id(++id_counter), text{std::move(other.text)}
    {
        ++count;
        std::print("Move constructor called, id = {}, count = {}\n", id, count);
    }

    Lifetime& operator=(const Lifetime& other)
    {
        std::print("Copy assignment operator called, id = {}, count = {}\n", id, count);
        if (this != &other) {
            text = other.text;
        }
        return *this;
    }

    Lifetime& operator=(Lifetime&& other) noexcept
    {
        std::print("Move assignment operator called, id = {}, count = {}\n", id, count);
        if (this != &other) {
            text = std::move(other.text);
        }
        return *this;
    }

    ~Lifetime() { std::print("Destructor called, id = {}, count = {}\n", id, --count); }

    inline static std::size_t id_counter = 0;
    inline static std::size_t count = 0;

    std::size_t id;
    std::string text{};
};

template <typename T>
void print_vector(const vector<T>& vec)
{
    for (auto i{0ul}; i < vec.size(); ++i) {
        std::print("{} ", vec[i]);
    }
    std::println("");
}

template <>
inline void print_vector<Lifetime>(const vector<Lifetime>& vec)
{
    for (auto i{0ul}; i < vec.size(); ++i) {
        std::print("id = {} (active={}, created={}) ", vec[i].id, vec[i].count, vec[i].id);
    }
    std::println("");
}

inline auto generate_random_vector(std::size_t n, int lowerBound, int upperBound)
{
    std::random_device rd;
    std::mt19937       gen{rd()};

    std::uniform_int_distribution<int> dist{lowerBound, upperBound};

    pstd::vector<int> result(n);
    std::ranges::generate(result, [&] { return dist(gen); });

    return result;
}

}  // namespace pstd::utilities
