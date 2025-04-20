add_executable(pstd_perf
    src/vector_perf.cpp
)

target_link_libraries(pstd_perf PRIVATE
    pstd::pstd
    benchmark::benchmark
    Threads::Threads
)
