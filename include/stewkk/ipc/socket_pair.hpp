#pragma once

#include <array>
#include <cstdint>

#include <sys/socket.h>

#include <stewkk/ipc/fdstreambuf.hpp>

namespace stewkk::ipc {

class SocketPair {
public:
  using ReadFD = std::int32_t;
  using WriteFD = std::int32_t;

  enum class Type { kStreamSocket = SOCK_STREAM, kDatagramSocket = SOCK_DGRAM };

  explicit SocketPair(Type type);

  FDBufIn GetReader();
  FDBufOut GetWriter();

private:
  std::array<std::int32_t, 2> fd_;
};

}  // namespace stewkk::ipc
