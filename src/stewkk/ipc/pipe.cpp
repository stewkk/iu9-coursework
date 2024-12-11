#include <stewkk/ipc/pipe.hpp>

#include <array>
#include <cstdint>
#include <tuple>
#include <utility>

#include <unistd.h>

#include <stewkk/ipc/errors.hpp>

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

} // namespace stewkk::ipc
