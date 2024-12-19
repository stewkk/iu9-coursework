#pragma once

#include <cstdint>
#include <optional>
#include <streambuf>
#include <utility>

#include <fcntl.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#include <stewkk/ipc/errors.hpp>
#include <stewkk/ipc/syscalls.hpp>

namespace stewkk::ipc {

template <std::size_t BufSize> class SystemvQueueBufIn;

template <std::size_t BufSize> class SystemvQueueBufOut;

template <std::size_t BufSize> class SystemvQueue {
public:
  SystemvQueue();

  SystemvQueueBufIn<BufSize> GetReader();
  SystemvQueueBufOut<BufSize> GetWriter();

private:
  std::int32_t fd_;
};

template <std::size_t BufSize> class SystemvQueueBufIn : public std::streambuf {
public:
  explicit SystemvQueueBufIn(std::int32_t fd);
  ~SystemvQueueBufIn();
  SystemvQueueBufIn(const SystemvQueueBufIn& other) = delete;
  SystemvQueueBufIn& operator=(const SystemvQueueBufIn& other) = delete;
  SystemvQueueBufIn(SystemvQueueBufIn&& other) noexcept;
  SystemvQueueBufIn& operator=(SystemvQueueBufIn&& other) noexcept;

  std::streamsize sgetn(char* buf, std::streamsize size);

private:
  int_type uflow() override;
  int_type underflow() override;

private:
  std::optional<std::int32_t> fd_;
  std::string name_;
};

template <std::size_t BufSize> class SystemvQueueBufOut : public std::streambuf {
public:
  explicit SystemvQueueBufOut(std::int32_t fd);

  std::streamsize sputn(const char* buf, std::streamsize size);

private:
  int_type overflow(int_type c) override;

private:
  std::int32_t fd_;
};

namespace {

template <std::size_t BufSize> struct Message {
  std::int64_t type;
  char data[BufSize];
};

template <std::size_t BufSize> bool Write(std::int32_t fd, const char* data, std::streamsize size) {
  Message<BufSize> message;
  message.type = 1;
  std::copy(data, data + size, message.data);
  auto ok = msgsnd(fd, &message, size, 0);
  if (ok == -1) {
    throw GetSyscallError();
  }

  return true;
}

std::int32_t MakeQueue() {
  auto id = msgget(IPC_PRIVATE, S_IRWXU | IPC_CREAT);
  if (id == -1) {
    throw GetSyscallError();
  }
  return id;
}

void RemoveQueue(std::int32_t fd) {
  auto res = msgctl(fd, IPC_RMID, nullptr);
  if (res == -1) {
    throw GetSyscallError();
  }
}

}  // namespace

template <std::size_t BufSize> SystemvQueueBufOut<BufSize>::SystemvQueueBufOut(std::int32_t fd)
    : fd_(fd) {}

template <std::size_t BufSize>
SystemvQueueBufOut<BufSize>::int_type SystemvQueueBufOut<BufSize>::overflow(int_type c) {
  return EOF;
}

template <std::size_t BufSize>
std::streamsize SystemvQueueBufOut<BufSize>::sputn(const char* buf, std::streamsize size) {
  auto res = Write<BufSize>(fd_, buf, size);
  return res;
}

template <std::size_t BufSize> SystemvQueueBufIn<BufSize>::SystemvQueueBufIn(std::int32_t fd)
    : fd_(fd) {}

template <std::size_t BufSize> SystemvQueueBufIn<BufSize>::~SystemvQueueBufIn() {
  if (fd_.has_value()) {
    RemoveQueue(fd_.value());
  }
}

template <std::size_t BufSize>
SystemvQueueBufIn<BufSize>::SystemvQueueBufIn(SystemvQueueBufIn&& other) noexcept
    : fd_(std::exchange(other.fd_, std::nullopt)), name_(std::move(other.name_)) {}

template <std::size_t BufSize> SystemvQueueBufIn<BufSize>& SystemvQueueBufIn<BufSize>::operator=(
    SystemvQueueBufIn&& other) noexcept {
  fd_ = std::exchange(other.fd_, std::nullopt);
  name_ = std::move(other.name_);
  return *this;
}

template <std::size_t BufSize>
SystemvQueueBufIn<BufSize>::int_type SystemvQueueBufIn<BufSize>::underflow() {
  return EOF;
}

template <std::size_t BufSize>
SystemvQueueBufIn<BufSize>::int_type SystemvQueueBufIn<BufSize>::uflow() {
  return EOF;
}

template <std::size_t BufSize>
std::streamsize SystemvQueueBufIn<BufSize>::sgetn(char* buf, std::streamsize size) {
  if (size < BufSize) {
    throw IpcError("buffer is smaller than BufSize");
  }

  Message<BufSize> message;

  auto read = msgrcv(fd_.value(), &message, BufSize, 0, 0);
  if (read == -1) {
    throw GetSyscallError();
  }
  if (read != BufSize) {
    throw IpcError("received less than BufSize bytes");
  }

  std::copy(message.data, message.data + read, buf);

  return read;
}

template <std::size_t BufSize> SystemvQueue<BufSize>::SystemvQueue() : fd_(MakeQueue()) {}

template <std::size_t BufSize> SystemvQueueBufOut<BufSize> SystemvQueue<BufSize>::GetWriter() {
  return SystemvQueueBufOut<BufSize>(fd_);
}

template <std::size_t BufSize> SystemvQueueBufIn<BufSize> SystemvQueue<BufSize>::GetReader() {
  return SystemvQueueBufIn<BufSize>(fd_);
}

}  // namespace stewkk::ipc
