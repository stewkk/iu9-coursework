#pragma once

#include <cstdint>
#include <optional>
#include <streambuf>

#include <mqueue.h>

namespace stewkk::ipc {

class PosixQueueBufIn;
class PosixQueueBufOut;

class PosixQueue {
public:
  PosixQueue(std::string name, std::int64_t max_messages, std::int64_t max_message_size);

  PosixQueueBufIn GetReader();
  PosixQueueBufOut GetWriter();

private:
  std::int32_t fd_;
  std::string name_;
};

class PosixQueueBufIn : public std::streambuf {
public:
  PosixQueueBufIn(std::int32_t fd, std::string name);
  ~PosixQueueBufIn();
  PosixQueueBufIn(const PosixQueueBufIn& other) = delete;
  PosixQueueBufIn& operator=(const PosixQueueBufIn& other) = delete;
  PosixQueueBufIn(PosixQueueBufIn&& other) noexcept;
  PosixQueueBufIn& operator=(PosixQueueBufIn&& other) noexcept;

  std::streamsize sgetn(char* buf, std::streamsize size);

private:
  int_type uflow() override;
  int_type underflow() override;

private:
  std::optional<std::int32_t> fd_;
  std::string name_;
};

class PosixQueueBufOut : public std::streambuf {
public:
  explicit PosixQueueBufOut(std::int32_t fd);
  ~PosixQueueBufOut();
  PosixQueueBufOut(const PosixQueueBufOut& other) = delete;
  PosixQueueBufOut& operator=(const PosixQueueBufOut& other) = delete;
  PosixQueueBufOut(PosixQueueBufOut&& other) noexcept;
  PosixQueueBufOut& operator=(PosixQueueBufOut&& other) noexcept;

  std::streamsize sputn(const char* buf, std::streamsize size);

private:
  int_type overflow(int_type c) override;

private:
  std::optional<std::int32_t> fd_;
};

}  // namespace stewkk::ipc
