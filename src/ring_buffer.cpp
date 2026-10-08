#include <new>
#include <array>
#include <atomic>
#include <cstddef>

#include <benchmark/benchmark.h>

enum class Alignment: std::size_t {
    Default = alignof(std::atomic<std::size_t>),
    Cacheline = std::hardware_destructive_interference_size
};

template <typename T,
          Alignment Align = Alignment::Default,
          std::size_t Size = 1024>
class RingBuffer {
public:
    __attribute__((noinline)) bool Push(const T& value) {
        const std::size_t writer = writer_.load(std::memory_order_relaxed);
        const std::size_t next_writer = (writer + 1) % Size;

        if (next_writer == reader_.load(std::memory_order_acquire))
            return false;

        buffer_[writer] = value;
        writer_.store(next_writer, std::memory_order_release);
        return true;
    }

    __attribute__((noinline)) bool Pop(T& value) {
        const std::size_t reader = reader_.load(std::memory_order_relaxed);
        const std::size_t next_reader = (reader + 1) % Size;

        if (reader == writer_.load(std::memory_order_acquire))
            return false;

        value = buffer_[reader];
        reader_.store(next_reader, std::memory_order_release);
        return true;
    }

private:
    alignas(Align) std::atomic<std::size_t> writer_{0};
    alignas(Align) std::atomic<std::size_t> reader_{0};
    alignas(Align) std::array<T, Size> buffer_;
};

// See: https://rigtorp.se/ringbuffer
template <typename T,
          Alignment Align = Alignment::Default,
          std::size_t Size = 1024>
class CachedRingBuffer {
public:
    __attribute__((noinline)) bool Push(const T& value) {
        const std::size_t writer = writer_.load(std::memory_order_relaxed);
        const std::size_t next_writer = (writer + 1) % Size;
        if (next_writer == cached_reader_) {
            cached_reader_ = reader_.load(std::memory_order_acquire);
            if (next_writer == cached_reader_)
                return false;
        }

        buffer_[writer] = value;
        writer_.store(next_writer, std::memory_order_release);
        return true;
    }

    __attribute__((noinline)) bool Pop(T& value) {
        const std::size_t reader = reader_.load(std::memory_order_relaxed);
        const std::size_t next_reader = (reader + 1) % Size;
        if (reader == cached_writer_) {
            cached_writer_ = writer_.load(std::memory_order_acquire);
            if (reader == cached_writer_)
                return false;
        }

        value = buffer_[reader];
        reader_.store(next_reader, std::memory_order_release);
        return true;
    }

private:
    alignas(Align) std::atomic<std::size_t> writer_{0};
    alignas(Align) std::size_t cached_reader_{0};

    alignas(Align) std::atomic<std::size_t> reader_{0};
    alignas(Align) std::size_t cached_writer_{0};

    alignas(Align) std::array<T, Size> buffer_;
};

static bool IsPusherThread(benchmark::State& state) {
    return state.thread_index() == 0;
}

template <typename RingBuffer>
static void DoPushPop(benchmark::State& state) {
    // Make ring_buffer static to share it between threads.
    static RingBuffer ring_buffer;

    if (IsPusherThread(state)) {
        int x = 0;
        for (auto _ : state) {
            while (!ring_buffer.Push(x))
                ++x;
        }
    } else {
        for (int x = 0; auto _ : state) {
            while (!ring_buffer.Pop(x)) {}
            benchmark::DoNotOptimize(x);
        }
    }
}

static void BM_PushPopNoAlign(benchmark::State& state) {
    DoPushPop<RingBuffer<int>>(state);
}
BENCHMARK(BM_PushPopNoAlign)->Threads(2);

static void BM_PushPopCachelineAlign(benchmark::State& state) {
    DoPushPop<RingBuffer<int, Alignment::Cacheline>>(state);
}
BENCHMARK(BM_PushPopCachelineAlign)->Threads(2);

static void BM_PushPopCachedNoAlign(benchmark::State& state) {
    DoPushPop<CachedRingBuffer<int>>(state);
}
BENCHMARK(BM_PushPopCachedNoAlign)->Threads(2);

static void BM_PushPopCachedCachelineAlign(benchmark::State& state) {
    DoPushPop<CachedRingBuffer<int, Alignment::Cacheline>>(state);
}
BENCHMARK(BM_PushPopCachedCachelineAlign)->Threads(2);

BENCHMARK_MAIN();
