find_package(GTest REQUIRED)

set(PSTD_TESTS pstd_tests)

add_executable(
  ${PSTD_TESTS}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/unique_ptr_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/vector_tests.cpp
)

target_include_directories(
    ${PSTD_TESTS}
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

target_link_libraries(
   ${PSTD_TESTS}
    PRIVATE GTest::gtest
            GTest::gmock
)

set(XML_OUTPUT_DIR ${CMAKE_BINARY_DIR}/Testing/Reports)
if(DEFINED ENV{GTEST_OUTPUT})
    set(XML_OUTPUT_DIR "")
endif()

include(GoogleTest)
gtest_discover_tests(
    ${PSTD_TESTS}
    DISCOVERY_MODE
    PRE_TEST
    XML_OUTPUT_DIR
    ${XML_OUTPUT_DIR}
)
