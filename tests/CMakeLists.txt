find_package(GTest REQUIRED)

add_executable(pstd_tests
    src/main.cpp
    src/binary_heap_tests.cpp
    src/iterator_tests.cpp
    src/ranges_tests.cpp
    src/unique_ptr_tests.cpp
    src/shared_ptr_tests.cpp
    src/vector_tests.cpp
)

target_link_libraries(pstd_tests PRIVATE
    GTest::gtest_main
    pstd::pstd
)

include(GoogleTest)
gtest_discover_tests(pstd_tests)
