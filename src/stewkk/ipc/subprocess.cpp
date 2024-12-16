#include <stewkk/ipc/subprocess.hpp>

#include <sys/wait.h>

#include <stewkk/ipc/errors.hpp>
#include <stewkk/ipc/fdstreambuf.hpp>
#include <stewkk/ipc/syscalls.hpp>

namespace stewkk::ipc {

Subprocess::~Subprocess() { Waitpid(child_pid_); }

}  // namespace stewkk::ipc
