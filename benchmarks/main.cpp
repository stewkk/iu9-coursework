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

constexpr std::size_t kOverallSize = 40000;
constexpr std::size_t kRangeFrom = 16;
constexpr std::size_t kRangeTo = 8 << 10;

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
  std::size_t size = state.range(0);
  std::size_t count = (kOverallSize + size - 1) / size;
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

BENCHMARK(BM_Pipe)->RangeMultiplier(2)->Range(kRangeFrom, kRangeTo);
;

static void BM_Fifo(benchmark::State& state) {
  std::size_t size = state.range(0);
  std::size_t count = (kOverallSize + size - 1) / size;
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

BENCHMARK(BM_Fifo)->RangeMultiplier(2)->Range(kRangeFrom, kRangeTo);
;

static void BM_StreamSocketPair(benchmark::State& state) {
  std::size_t size = state.range(0);
  std::size_t count = (kOverallSize + size - 1) / size;
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

BENCHMARK(BM_StreamSocketPair)->RangeMultiplier(2)->Range(kRangeFrom, kRangeTo);
;

static void BM_DatagramSocketPair(benchmark::State& state) {
  std::size_t size = state.range(0);
  std::size_t count = (kOverallSize + size - 1) / size;
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

BENCHMARK(BM_DatagramSocketPair)->RangeMultiplier(2)->Range(kRangeFrom, kRangeTo);
;

static void BM_PosixQueue(benchmark::State& state) {
  std::size_t size = state.range(0);
  std::size_t count = (kOverallSize + size - 1) / size;
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

BENCHMARK(BM_PosixQueue)->RangeMultiplier(2)->Range(kRangeFrom, kRangeTo);
;

template <std::size_t BufSize> static void BM_SystemvQueue(benchmark::State& state) {
  std::size_t size = state.range(0);
  std::size_t count = (kOverallSize + size - 1) / size;
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

BENCHMARK(BM_SystemvQueue<16>)->Arg(16);
BENCHMARK(BM_SystemvQueue<32>)->Arg(32);
BENCHMARK(BM_SystemvQueue<64>)->Arg(64);
BENCHMARK(BM_SystemvQueue<128>)->Arg(128);
BENCHMARK(BM_SystemvQueue<256>)->Arg(256);
BENCHMARK(BM_SystemvQueue<512>)->Arg(512);
BENCHMARK(BM_SystemvQueue<1024>)->Arg(1024);
BENCHMARK(BM_SystemvQueue<2048>)->Arg(2048);
BENCHMARK(BM_SystemvQueue<4096>)->Arg(4096);
BENCHMARK(BM_SystemvQueue<8192>)->Arg(8192);

static void BM_AnonMemMapping(benchmark::State& state) {
  std::size_t size = state.range(0);
  std::size_t count = (kOverallSize + size - 1) / size;
  auto message = GenerateRandomMessage(size);

  std::string got(message.size(), ' ');

  for (auto _ : state) {
    Subprocess<MemMappingBufIn, MemMappingBufOut> child(
        [&count, &message](MemMappingBufOut out) {
          for (std::size_t i = 0; i < count; ++i) {
            out.sputn(message.data(), message.size());
          }
        },
        MemMapping(MakeAnonymousMapping, message.size() * count));
    for (std::size_t i = 0; i < count; ++i) {
      child.stdout.sgetn(got.data(), got.size());
    }
  }
}

BENCHMARK(BM_AnonMemMapping)->RangeMultiplier(2)->Range(kRangeFrom, kRangeTo);
;

static void BM_FileMemMapping(benchmark::State& state) {
  std::size_t size = state.range(0);
  std::size_t count = (kOverallSize + size - 1) / size;
  auto message = GenerateRandomMessage(size);

  std::string got(message.size(), ' ');

  for (auto _ : state) {
    Subprocess<MemMappingBufIn, MemMappingBufOut> child(
        [&count, &message](MemMappingBufOut out) {
          for (std::size_t i = 0; i < count; ++i) {
            out.sputn(message.data(), message.size());
          }
        },
        MemMapping(MakeFileMapping, message.size() * count));
    for (std::size_t i = 0; i < count; ++i) {
      child.stdout.sgetn(got.data(), got.size());
    }
  }
}

BENCHMARK(BM_FileMemMapping)->RangeMultiplier(2)->Range(kRangeFrom, kRangeTo);
;

static void BM_PosixSharedMemory(benchmark::State& state) {
  std::size_t size = state.range(0);
  std::size_t count = (kOverallSize + size - 1) / size;
  auto message = GenerateRandomMessage(size);

  std::string got(message.size(), ' ');

  for (auto _ : state) {
    Subprocess<MemMappingBufIn, MemMappingBufOut> child(
        [&count, &message](MemMappingBufOut out) {
          for (std::size_t i = 0; i < count; ++i) {
            out.sputn(message.data(), message.size());
          }
        },
        MemMapping(MakePosixSharedMemory, message.size() * count));
    for (std::size_t i = 0; i < count; ++i) {
      child.stdout.sgetn(got.data(), got.size());
    }
  }
}

BENCHMARK(BM_PosixSharedMemory)->RangeMultiplier(2)->Range(kRangeFrom, kRangeTo);
;

}  // namespace stewkk::ipc

BENCHMARK_MAIN();
