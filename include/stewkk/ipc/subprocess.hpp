#pragma once

#include <concepts>
#include <cstdlib>
#include <functional>
#include <streambuf>

#include <stewkk/ipc/fdstreambuf.hpp>
#include <stewkk/ipc/mmap.hpp>
#include <stewkk/ipc/posix_queue.hpp>
#include <stewkk/ipc/syscalls.hpp>

namespace stewkk::ipc {

template <typename I>
concept StreamBuf = requires(I i) { std::derived_from<I, std::streambuf>; };

template <typename I>
concept RWIpc = requires(I i) {
  { i.GetReader() } -> StreamBuf;
  { i.GetWriter() } -> StreamBuf;
};

template <StreamBuf InBuf, StreamBuf OutBuf> class Subprocess {
public:
  template <RWIpc Ipc> Subprocess(std::function<void(OutBuf)> subprogram, Ipc ipc);
  ~Subprocess();
  Subprocess(const Subprocess& other) = delete;
  Subprocess& operator=(const Subprocess& other) = delete;

  InBuf stdout;

private:
  pid_t child_pid_;
};

template <StreamBuf InBuf, StreamBuf OutBuf>
Subprocess<InBuf, OutBuf>::Subprocess(std::function<void(OutBuf)> subprogram, RWIpc auto ipc)
    : stdout(-1) {
  auto pid = Fork();

  if (pid == 0) {
    subprogram(ipc.GetWriter());
    std::exit(0);
  }

  child_pid_ = pid;
  stdout = ipc.GetReader();
}

template <> template <RWIpc Ipc> Subprocess<PosixQueueBufIn, PosixQueueBufOut>::Subprocess(
    std::function<void(PosixQueueBufOut)> subprogram, Ipc ipc)
    : stdout(-1, "") {
  auto pid = Fork();

  if (pid == 0) {
    subprogram(ipc.GetWriter());
    std::exit(0);
  }

  child_pid_ = pid;
  stdout = ipc.GetReader();
}

template <> template <RWIpc Ipc> Subprocess<MemMappingBufIn, MemMappingBufOut>::Subprocess(
    std::function<void(MemMappingBufOut)> subprogram, Ipc ipc)
    : stdout(0, nullptr) {
  auto pid = Fork();

  if (pid == 0) {
    subprogram(ipc.GetWriter());
    std::exit(0);
  }

  child_pid_ = pid;
  stdout = ipc.GetReader();
}

template <StreamBuf InBuf, StreamBuf OutBuf> Subprocess<InBuf, OutBuf>::~Subprocess() {
  Waitpid(child_pid_);
}

}  // namespace stewkk::ipc
