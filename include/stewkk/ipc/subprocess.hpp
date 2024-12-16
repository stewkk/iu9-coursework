#pragma once

#include <cstdlib>
#include <functional>

#include <stewkk/ipc/fdstreambuf.hpp>
#include <stewkk/ipc/syscalls.hpp>

namespace stewkk::ipc {

template <typename I>
concept RWIpc = requires(I i) {
  { i.GetReader() } -> std::same_as<FDBufIn>;
  { i.GetWriter() } -> std::same_as<FDBufOut>;
};

class Subprocess {
public:
  Subprocess(std::function<void(FDBufOut)> subprogram, RWIpc auto ipc);
  ~Subprocess();
  Subprocess(const Subprocess& other) = delete;
  Subprocess& operator=(const Subprocess& other) = delete;

  FDBufIn stdout;

private:
  pid_t child_pid_;
};

Subprocess::Subprocess(std::function<void(FDBufOut)> subprogram, RWIpc auto ipc) : stdout(-1) {
  auto pid = Fork();

  if (pid == 0) {
    subprogram(ipc.GetWriter());
    std::exit(0);
  }

  child_pid_ = pid;
  stdout = ipc.GetReader();
}

}  // namespace stewkk::ipc
