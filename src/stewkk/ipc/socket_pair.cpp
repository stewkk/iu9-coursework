#include <stewkk/ipc/socket_pair.hpp>

#include <array>

#include <sys/socket.h>

#include <stewkk/ipc/errors.hpp>
#include <stewkk/ipc/syscalls.hpp>

namespace stewkk::ipc {

namespace {

std::array<std::int32_t, 2> MakeSocketPair(SocketPair::Type type) {
  std::array<std::int32_t, 2> fd;
  auto res = socketpair(AF_UNIX, static_cast<std::int32_t>(type), 0, fd.data());
  if (res == -1) {
    throw GetSyscallError();
  }

  return fd;
}

}  // namespace

SocketPair::SocketPair(Type type) : fd_(MakeSocketPair(type)) {}

FDBufIn SocketPair::GetReader() {
  Close(fd_[0]);
  return FDBufIn(fd_[1]);
}

FDBufOut SocketPair::GetWriter() {
  Close(fd_[1]);
  return FDBufOut(fd_[0]);
}

}  // namespace stewkk::ipc
