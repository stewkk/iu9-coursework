#pragma once

#include <cstdint>
#include <filesystem>

#include <stewkk/ipc/fdstreambuf.hpp>

namespace stewkk::ipc {

class Fifo {
public:
  using ReadFD = std::int32_t;
  using WriteFD = std::int32_t;

  explicit Fifo(std::filesystem::path path);

  FDBufIn GetReader();
  FDBufOut GetWriter();

private:
  std::filesystem::path path_;
};

}  // namespace stewkk::ipc
