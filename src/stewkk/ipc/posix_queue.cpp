#include <stewkk/ipc/posix_queue.hpp>

#include <mqueue.h>
#include <unistd.h>

#include <utility>

#include <stewkk/ipc/errors.hpp>
#include <stewkk/ipc/syscalls.hpp>

namespace stewkk::ipc {

namespace {

bool Write(std::int32_t fd, char data) {
  auto ok = mq_send(fd, &data, sizeof(data), 0);
  if (ok == -1) {
    throw GetSyscallError();
  }

  return true;
}

bool Write(std::int32_t fd, const char* data, std::streamsize size) {
  auto ok = mq_send(fd, data, size, 0);

  if (ok == -1) {
    throw GetSyscallError();
  }

  return true;
}

std::int32_t MqOpen(std::string name, std::int64_t max_messages, std::int64_t max_message_size) {
  struct mq_attr attr {
    .mq_flags = 0, .mq_maxmsg = max_messages, .mq_msgsize = max_message_size, .mq_curmsgs = 0,
  };
  auto fd = mq_open(name.c_str(), O_CREAT | O_RDWR, S_IRWXU, &attr);
  if (fd == -1) {
    throw GetSyscallError();
  }
  return fd;
}

void MqClose(std::int32_t fd) {
  auto res = mq_close(fd);
  if (res == -1) {
    throw GetSyscallError();
  }
}

void MqUnlink(std::string name) {
  auto res = mq_unlink(name.c_str());
  if (res == -1) {
    throw GetSyscallError();
  }
}

}  // namespace

PosixQueueBufOut::PosixQueueBufOut(std::int32_t fd) : fd_(fd) {}

PosixQueueBufOut::~PosixQueueBufOut() {
  if (fd_.has_value()) {
    MqClose(fd_.value());
  }
}

PosixQueueBufOut::PosixQueueBufOut(PosixQueueBufOut&& other) noexcept
    : fd_(std::exchange(other.fd_, std::nullopt)) {}

PosixQueueBufOut& PosixQueueBufOut::operator=(PosixQueueBufOut&& other) noexcept {
  fd_ = std::exchange(other.fd_, std::nullopt);
  return *this;
}

PosixQueueBufOut::int_type PosixQueueBufOut::overflow(int_type c) { return EOF; }

std::streamsize PosixQueueBufOut::sputn(const char* buf, std::streamsize size) {
  auto res = Write(fd_.value(), buf, size);
  return res;
}

PosixQueueBufIn::PosixQueueBufIn(std::int32_t fd, std::string name)
    : fd_(fd), name_(std::move(name)) {}

PosixQueueBufIn::~PosixQueueBufIn() {
  if (fd_.has_value()) {
    MqClose(fd_.value());
  }
}

PosixQueueBufIn::PosixQueueBufIn(PosixQueueBufIn&& other) noexcept
    : fd_(std::exchange(other.fd_, std::nullopt)), name_(std::move(other.name_)) {}

PosixQueueBufIn& PosixQueueBufIn::operator=(PosixQueueBufIn&& other) noexcept {
  fd_ = std::exchange(other.fd_, std::nullopt);
  name_ = std::move(other.name_);
  return *this;
}

PosixQueueBufIn::int_type PosixQueueBufIn::underflow() { return EOF; }

PosixQueueBufIn::int_type PosixQueueBufIn::uflow() { return EOF; }

std::streamsize PosixQueueBufIn::sgetn(char* buf, std::streamsize size) {
  auto read = mq_receive(fd_.value(), buf, size, nullptr);
  if (read == -1) {
    throw GetSyscallError();
  }
  return read;
}

PosixQueue::PosixQueue(std::string name, std::int64_t max_messages, std::int64_t max_message_size)
    : fd_(MqOpen(name, max_messages, max_message_size)), name_(name) {
  MqUnlink(name);
}

PosixQueueBufOut PosixQueue::GetWriter() { return PosixQueueBufOut(fd_); }

PosixQueueBufIn PosixQueue::GetReader() { return PosixQueueBufIn(fd_, name_); }

}  // namespace stewkk::ipc
