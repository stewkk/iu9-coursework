#include <benchmark/benchmark.h>

#include <random>
#include <limits>
#include <filesystem>

#include <stewkk/ipc/fdstreambuf.hpp>
#include <stewkk/ipc/subprocess.hpp>
#include <stewkk/ipc/pipe.hpp>
#include <stewkk/ipc/fifo.hpp>

namespace stewkk::ipc {

static void BM_Pipe(benchmark::State& state) {
  std::size_t count = state.range(0);
  std::size_t size = state.range(0);
  std::string message(size, ' ');

  std::random_device rd;
  static std::mt19937 generator(rd());
  std::uniform_int_distribution<char> distribution(std::numeric_limits<char>::min(), std::numeric_limits<char>::max());
  for (auto& el : message) {
    el = distribution(generator);
  }

  Subprocess child([&count, &message](FDBufOut out){
    for (std::size_t i = 0; i < count; ++i) {
      out.sputn(message.data(), message.size());
    }
  }, Pipe());

  std::string got(message.size(), ' ');

  for (auto _ : state) {
    for (std::size_t i = 0; i < count; ++i) {
      child.stdout.sgetn(got.data(), got.size());
    }
  }
}

BENCHMARK(BM_Pipe)->RangeMultiplier(2)->Range(8, 8<<10);;

static void BM_Fifo(benchmark::State& state) {
  std::size_t count = state.range(0);
  std::size_t size = state.range(0);
  std::string message(size, ' ');

  std::random_device rd;
  static std::mt19937 generator(rd());
  std::uniform_int_distribution<char> distribution(std::numeric_limits<char>::min(), std::numeric_limits<char>::max());
  for (auto& el : message) {
    el = distribution(generator);
  }

  auto path = std::filesystem::temp_directory_path() / "coursework-fifo-benchmark";
  std::filesystem::remove(path);
  Subprocess child([&count, &message](FDBufOut out){
    for (std::size_t i = 0; i < count; ++i) {
      out.sputn(message.data(), message.size());
    }
  }, Fifo(std::filesystem::temp_directory_path() / "coursework-fifo-benchmark"));

  std::string got(message.size(), ' ');

  for (auto _ : state) {
    for (std::size_t i = 0; i < count; ++i) {
      child.stdout.sgetn(got.data(), got.size());
    }
  }
}

BENCHMARK(BM_Fifo)->RangeMultiplier(2)->Range(8, 8<<10);;

}  // namespace stewkk::ipc

BENCHMARK_MAIN();
