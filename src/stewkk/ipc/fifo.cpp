#include <stewkk/ipc/fifo.hpp>

#include <filesystem>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stewkk/ipc/errors.hpp>
#include <stewkk/ipc/syscalls.hpp>

namespace stewkk::ipc {

namespace {

void MakeFifo(std::filesystem::path path) {
  auto ret = mkfifo(path.c_str(), S_IRWXU);
  if (ret == -1) {
      throw GetSyscallError();
  }
}

} // namespace

Fifo::Fifo(std::filesystem::path path) : path_(path) {
    MakeFifo(path);
}

FDBufIn Fifo::GetReader() {
    auto fd = Open(path_, O_RDONLY);
    return FDBufIn(fd);
}

FDBufOut Fifo::GetWriter() {
    auto fd = Open(path_, O_WRONLY);
    return FDBufOut(fd);
}

} // namespace stewkk::ipc
