#include <string>

#include <benchmark/benchmark.h>

static void BM_StringLiteral(benchmark::State& state) {
    // Idea is to keep strlen() call not being optimized out/constant folded.
    const char* p = "";
    benchmark::DoNotOptimize(p);
    for (auto _ : state) {
        benchmark::DoNotOptimize(std::string(p));
    }
}
BENCHMARK(BM_StringLiteral);

static void BM_StringLiteralFolding(benchmark::State& state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(std::string(""));
    }
}
BENCHMARK(BM_StringLiteralFolding);

static void BM_Braces(benchmark::State& state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(std::string({}));
    }
}
BENCHMARK(BM_Braces);

BENCHMARK_MAIN();
