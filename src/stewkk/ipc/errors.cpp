#include <stewkk/ipc/errors.hpp>

#include <cerrno>
#include <cstring>
#include <format>

namespace stewkk::ipc {

SyscallError GetSyscallError() {
   auto err = errno;
   return SyscallError(err, std::system_category(), std::format("{}", strerrorname_np(err)));
}

}  // namespace stewkk::ipc
