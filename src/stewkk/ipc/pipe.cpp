#include <stewkk/ipc/pipe.hpp>

#include <array>
#include <cstdint>
#include <tuple>
#include <utility>

#include <unistd.h>

#include <stewkk/ipc/errors.hpp>
#include <stewkk/ipc/syscalls.hpp>

namespace stewkk::ipc {

namespace {

std::pair<Pipe::ReadFD, Pipe::WriteFD> MakePipe() {
  std::array<std::int32_t, 2> pipe_file_descriptors;
  if (pipe(pipe_file_descriptors.data()) == -1) {
    throw GetSyscallError();
  }
  return {
      pipe_file_descriptors[0],
      pipe_file_descriptors[1],
  };
}

} // namespace

Pipe::Pipe() {
    std::tie(read_fd_, write_fd_) = MakePipe();
}

FDBufIn Pipe::GetReader() {
    Close(write_fd_);
    return FDBufIn(read_fd_);
}

FDBufOut Pipe::GetWriter() {
    Close(read_fd_);
    return FDBufOut(write_fd_);
}

} // namespace stewkk::ipc
