#include <benchmark/benchmark.h>
#include <limits>
#include <random>

#include <pstd/vector/vector.hpp>
#include <vector>

std::vector<int> generate_random_ints(size_t count)
{
    std::vector<int> data;
    data.reserve(count);
    std::random_device              rd;
    std::mt19937                    gen(rd());
    std::uniform_int_distribution<> distrib(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    for (size_t i = 0; i < count; ++i) {
        data.push_back(distrib(gen));
    }
    return data;
}

// Benchmark pstd::vector push_back without pre-allocation
static void BM_PstdVectorPushBack(benchmark::State& state)
{
    long n = state.range(0);
    for (auto _ : state) {
        pstd::vector<int> v;
        for (long i = 0; i < n; ++i) {
            v.push_back(i);
        }
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * n);
    state.SetComplexityN(n);
}
BENCHMARK(BM_PstdVectorPushBack)->RangeMultiplier(8)->Range(1 << 10, 1 << 20)->Complexity();

static void BM_StdVectorPushBack(benchmark::State& state)
{
    long n = state.range(0);
    for (auto _ : state) {
        std::vector<int> v;
        for (long i = 0; i < n; ++i) {
            v.push_back(i);
        }
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * n);
    state.SetComplexityN(n);
}
BENCHMARK(BM_StdVectorPushBack)->RangeMultiplier(8)->Range(1 << 10, 1 << 20)->Complexity();

// Benchmark pstd::vector push_back with reserve
static void BM_PstdVectorPushBackReserved(benchmark::State& state)
{
    long n = state.range(0);
    for (auto _ : state) {
        state.PauseTiming();  // Don't time the reserve itself
        pstd::vector<int> v;
        v.reserve(n);
        benchmark::DoNotOptimize(v.data());
        state.ResumeTiming();

        for (long i = 0; i < n; ++i) {
            v.push_back(i);
        }
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * n);
    state.SetComplexityN(n);
}
BENCHMARK(BM_PstdVectorPushBackReserved)->RangeMultiplier(8)->Range(1 << 10, 1 << 20)->Complexity();

// Benchmark std::vector push_back with reserve
static void BM_StdVectorPushBackReserved(benchmark::State& state)
{
    long n = state.range(0);
    for (auto _ : state) {
        state.PauseTiming();
        std::vector<int> v;
        v.reserve(n);
        benchmark::DoNotOptimize(v.data());
        state.ResumeTiming();

        for (long i = 0; i < n; ++i) {
            v.push_back(i);
        }
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * n);
    state.SetComplexityN(n);
}
BENCHMARK(BM_StdVectorPushBackReserved)->RangeMultiplier(8)->Range(1 << 10, 1 << 20)->Complexity();

// Example: Construction
static void BM_PstdVectorConstructSize(benchmark::State& state)
{
    long n = state.range(0);
    for (auto _ : state) {
        pstd::vector<int> v(n);
        benchmark::DoNotOptimize(v.data());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations());
    state.SetComplexityN(n);
}
BENCHMARK(BM_PstdVectorConstructSize)->RangeMultiplier(8)->Range(1 << 10, 1 << 20)->Complexity();

static void BM_StdVectorConstructSize(benchmark::State& state)
{
    long n = state.range(0);
    for (auto _ : state) {
        std::vector<int> v(n);
        benchmark::DoNotOptimize(v.data());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations());
    state.SetComplexityN(n);
}
BENCHMARK(BM_StdVectorConstructSize)->RangeMultiplier(8)->Range(1 << 10, 1 << 20)->Complexity();

BENCHMARK_MAIN();
