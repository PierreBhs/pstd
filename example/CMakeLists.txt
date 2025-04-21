cmake_minimum_required(VERSION 3.28)
project(pstd_example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wno-missing-field-initializers)

find_package(pstd REQUIRED)

add_executable(example src/main.cpp)
target_link_libraries(example pstd::pstd)

install(TARGETS example DESTINATION bin)
