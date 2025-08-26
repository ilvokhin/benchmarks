#include <benchmark/benchmark.h>

static void BM_Noop(benchmark::State& state) {
  for (auto _ : state) {
    __asm__("nop");
  }
}
BENCHMARK(BM_Noop);

BENCHMARK_MAIN();
