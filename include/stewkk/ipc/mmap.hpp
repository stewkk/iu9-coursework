#pragma once

#include <streambuf>

namespace stewkk::ipc {

class MemMappingBufIn;
class MemMappingBufOut;

class MemMapping {
public:
  explicit MemMapping(std::size_t length);

  MemMappingBufIn GetReader();
  MemMappingBufOut GetWriter();

private:
  std::size_t length_;
  char* addr_;
};

class MemMappingBufIn : public std::streambuf {
public:
  MemMappingBufIn(std::size_t length, char* addr);
  ~MemMappingBufIn();
  MemMappingBufIn(const MemMappingBufIn& other) = delete;
  MemMappingBufIn& operator=(const MemMappingBufIn& other) = delete;
  MemMappingBufIn(MemMappingBufIn&& other) noexcept;
  MemMappingBufIn& operator=(MemMappingBufIn&& other) noexcept;

  std::streamsize sgetn(char* buf, std::streamsize size);

private:
  int_type uflow() override;
  int_type underflow() override;

private:
  char* addr_;
  char* cur_addr_;
  std::size_t length_;
};

class MemMappingBufOut : public std::streambuf {
public:
  MemMappingBufOut(std::size_t length, char* addr);
  ~MemMappingBufOut();
  MemMappingBufOut(const MemMappingBufOut& other) = delete;
  MemMappingBufOut& operator=(const MemMappingBufOut& other) = delete;
  MemMappingBufOut(MemMappingBufOut&& other) noexcept;
  MemMappingBufOut& operator=(MemMappingBufOut&& other) noexcept;

  std::streamsize sputn(const char* buf, std::streamsize size);

private:
  int_type overflow(int_type c) override;

private:
  char* addr_;
  char* cur_addr_;
  std::size_t length_;
};

}  // namespace stewkk::ipc
