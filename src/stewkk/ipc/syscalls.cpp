#include <stewkk/ipc/syscalls.hpp>

#include <cstdint>

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

}  // namespace stewkk::ipc
