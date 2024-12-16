#pragma once

#include <system_error>

namespace stewkk::ipc {

class SyscallError : public std::system_error {
  using std::system_error::system_error;
};

SyscallError GetSyscallError();

}  // namespace stewkk::ipc
