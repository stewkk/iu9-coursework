#pragma once

#include <system_error>

namespace stewkk::ipc {

class SyscallError : public std::system_error {
  using std::system_error::system_error;
};

SyscallError GetSyscallError();

class IpcError : public std::logic_error {
  using std::logic_error::logic_error;
};

}  // namespace stewkk::ipc
