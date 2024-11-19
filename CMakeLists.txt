cmake_minimum_required(VERSION 3.28)
project(pstd)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wno-missing-field-initializers)

include_directories(${CMAKE_SOURCE_DIR}/include)


add_executable(${PROJECT_NAME}
                ${CMAKE_SOURCE_DIR}/src/main.cpp
)

# target_link_libraries(${PROJECT_NAME} PRIVATE raylib raygui)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
)
