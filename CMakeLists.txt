cmake_minimum_required(VERSION 3.28)
project(pstd VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")

add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wno-missing-field-initializers)

add_library(pstd INTERFACE)
add_library(pstd::pstd ALIAS pstd)

target_include_directories(pstd INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(pstd INTERFACE cxx_std_23)

# --- Installation ---
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(DIRECTORY include/pstd/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS pstd
    EXPORT pstdTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT pstdTargets
    FILE pstdTargets.cmake
    NAMESPACE pstd:: # Consumers will use pstd::pstd
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pstd
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/pstdConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/pstdConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/pstdConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pstd
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/pstdConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/pstdConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pstd
)

# --- Unit Tests ---
include(CTest)
option(PSTD_BUILD_TESTS "Build the pstd unit tests" ON)
if(PSTD_BUILD_TESTS)
    add_subdirectory(tests)
endif()

# --- Performance Tests ---
option(PSTD_BUILD_PERF_TESTS "Build the pstd performance tests" OFF)
if(PSTD_BUILD_PERF_TESTS)
    find_package(benchmark REQUIRED)
    find_package(Threads REQUIRED)
    add_subdirectory(perf)
endif()

execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
    RESULT_VARIABLE symlink_result
    OUTPUT_QUIET ERROR_QUIET
)
if(NOT symlink_result EQUAL 0)
    message(WARNING "Could not create symlink for compile_commands.json.")
endif()
