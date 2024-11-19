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

}  // namespace pstd::utilities
