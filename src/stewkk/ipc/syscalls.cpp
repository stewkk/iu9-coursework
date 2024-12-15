#include <stewkk/ipc/syscalls.hpp>

#include <cstdint>

#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#include <stewkk/ipc/errors.hpp>

namespace stewkk::ipc {

pid_t Fork() {
    auto res = fork();
    if (res == -1) {
        throw GetSyscallError();
    }

    return res;
}

void Waitpid(pid_t child_pid) {
  auto res = waitpid(child_pid, nullptr, 0);
  if (res == -1) {
      throw GetSyscallError();
  }
}

void Close(std::int32_t fd) {
    auto res = close(fd);
    if (res == -1) {
        throw GetSyscallError();
    }
}

std::int32_t Open(std::filesystem::path path, std::int32_t flags) {
  auto fd = open(path.c_str(), flags);
  if (fd == -1) {
    throw GetSyscallError();
  }
  return fd;
}

}  // namespace stewkk::ipc
