#include <benchmark/benchmark.h>

#include <filesystem>
#include <limits>
#include <random>

#include <stewkk/ipc/fdstreambuf.hpp>
#include <stewkk/ipc/fifo.hpp>
#include <stewkk/ipc/mmap.hpp>
#include <stewkk/ipc/pipe.hpp>
#include <stewkk/ipc/posix_queue.hpp>
#include <stewkk/ipc/socket_pair.hpp>
#include <stewkk/ipc/subprocess.hpp>
#include <stewkk/ipc/systemv_queue.hpp>

namespace stewkk::ipc {

namespace {

std::string GenerateRandomMessage(std::size_t size) {
  std::string message(size, ' ');
  static std::mt19937 generator(123);
  std::uniform_int_distribution<char> distribution(std::numeric_limits<char>::min(),
                                                   std::numeric_limits<char>::max());
  for (auto& el : message) {
    el = distribution(generator);
  }

  return message;
}

}  // namespace

static void BM_Pipe(benchmark::State& state) {
  std::size_t count = 1000;
  std::size_t size = state.range(0);
  auto message = GenerateRandomMessage(size);

  std::string got(message.size(), ' ');

  for (auto _ : state) {
    Subprocess<FDBufIn, FDBufOut> child(
        [&count, &message](FDBufOut out) {
          for (std::size_t i = 0; i < count; ++i) {
            out.sputn(message.data(), message.size());
          }
        },
        Pipe());
    for (std::size_t i = 0; i < count; ++i) {
      child.stdout.sgetn(got.data(), got.size());
    }
  }
}

BENCHMARK(BM_Pipe)->RangeMultiplier(2)->Range(8, 8 << 1);
;

static void BM_Fifo(benchmark::State& state) {
  std::size_t count = 1000;
  std::size_t size = state.range(0);
  auto message = GenerateRandomMessage(size);

  auto path = std::filesystem::temp_directory_path() / "coursework-fifo-benchmark";
  std::string got(message.size(), ' ');

  for (auto _ : state) {
    std::filesystem::remove(path);
    Subprocess<FDBufIn, FDBufOut> child(
        [&count, &message](FDBufOut out) {
          for (std::size_t i = 0; i < count; ++i) {
            out.sputn(message.data(), message.size());
          }
        },
        Fifo(std::filesystem::temp_directory_path() / "coursework-fifo-benchmark"));
    for (std::size_t i = 0; i < count; ++i) {
      child.stdout.sgetn(got.data(), got.size());
    }
  }
}

BENCHMARK(BM_Fifo)->RangeMultiplier(2)->Range(8, 8 << 1);
;

static void BM_StreamSocketPair(benchmark::State& state) {
  std::size_t count = 1000;
  std::size_t size = state.range(0);
  auto message = GenerateRandomMessage(size);
  std::string got(message.size(), ' ');

  for (auto _ : state) {
    Subprocess<FDBufIn, FDBufOut> child(
        [&count, &message](FDBufOut out) {
          for (std::size_t i = 0; i < count; ++i) {
            out.sputn(message.data(), message.size());
          }
        },
        SocketPair(SocketPair::Type::kStreamSocket));
    for (std::size_t i = 0; i < count; ++i) {
      child.stdout.sgetn(got.data(), got.size());
    }
  }
}

BENCHMARK(BM_StreamSocketPair)->RangeMultiplier(2)->Range(8, 8 << 1);
;

static void BM_DatagramSocketPair(benchmark::State& state) {
  std::size_t count = 1000;
  std::size_t size = state.range(0);
  auto message = GenerateRandomMessage(size);
  std::string got(message.size(), ' ');

  for (auto _ : state) {
    Subprocess<FDBufIn, FDBufOut> child(
        [&count, &message](FDBufOut out) {
          for (std::size_t i = 0; i < count; ++i) {
            out.sputn(message.data(), message.size());
          }
        },
        SocketPair(SocketPair::Type::kStreamSocket));
    for (std::size_t i = 0; i < count; ++i) {
      child.stdout.sgetn(got.data(), got.size());
    }
  }
}

BENCHMARK(BM_DatagramSocketPair)->RangeMultiplier(2)->Range(8, 8 << 1);
;

static void BM_PosixQueue(benchmark::State& state) {
  std::size_t count = 1000;
  std::size_t size = state.range(0);
  auto message = GenerateRandomMessage(size);
  std::string got(message.size(), ' ');

  for (auto _ : state) {
    Subprocess<PosixQueueBufIn, PosixQueueBufOut> child(
        [&count, &message](PosixQueueBufOut out) {
          for (std::size_t i = 0; i < count; ++i) {
            out.sputn(message.data(), message.size());
          }
        },
        PosixQueue("/stewkk-ipc-benchmark", 10, size));
    for (std::size_t i = 0; i < count; ++i) {
      child.stdout.sgetn(got.data(), got.size());
    }
  }
}

BENCHMARK(BM_PosixQueue)->RangeMultiplier(2)->Range(8, 8 << 1);
;

template <std::size_t BufSize> static void BM_SystemvQueue(benchmark::State& state) {
  std::size_t count = 1000;
  std::size_t size = state.range(0);
  auto message = GenerateRandomMessage(size);

  std::string got(message.size(), ' ');

  for (auto _ : state) {
    Subprocess<SystemvQueueBufIn<BufSize>, SystemvQueueBufOut<BufSize>> child(
        [&count, &message](SystemvQueueBufOut<BufSize> out) {
          for (std::size_t i = 0; i < count; ++i) {
            out.sputn(message.data(), message.size());
          }
        },
        SystemvQueue<BufSize>());
    for (std::size_t i = 0; i < count; ++i) {
      child.stdout.sgetn(got.data(), got.size());
    }
  }
}

BENCHMARK(BM_SystemvQueue<8>)->Arg(8);
;
BENCHMARK(BM_SystemvQueue<16>)->Arg(16);

static void BM_MemMapping(benchmark::State& state) {
  std::size_t count = 1000;
  std::size_t size = state.range(0);
  auto message = GenerateRandomMessage(size);

  std::string got(message.size(), ' ');

  for (auto _ : state) {
    Subprocess<MemMappingBufIn, MemMappingBufOut> child(
        [&count, &message](MemMappingBufOut out) {
          for (std::size_t i = 0; i < count; ++i) {
            out.sputn(message.data(), message.size());
          }
        },
        MemMapping(message.size() * count));
    for (std::size_t i = 0; i < count; ++i) {
      child.stdout.sgetn(got.data(), got.size());
    }
  }
}

BENCHMARK(BM_MemMapping)->RangeMultiplier(2)->Range(8, 8 << 1);
;

}  // namespace stewkk::ipc

BENCHMARK_MAIN();
