#pragma once

#include <concepts>
#include <streambuf>

#include <semaphore.h>
#include <sys/mman.h>

#include <stewkk/ipc/errors.hpp>

namespace stewkk::ipc {

class MemMappingBufIn;
class MemMappingBufOut;

template <typename M>
concept MemMappingMaker = requires(M mmaker, std::size_t size) {
  { mmaker(size) } -> std::same_as<char*>;
};

class MemMapping {
public:
  template <typename MakeMapping> MemMapping(MakeMapping mapping_maker, std::size_t length);

  MemMappingBufIn GetReader();
  MemMappingBufOut GetWriter();

private:
  std::size_t length_;
  char* addr_;
};

namespace {

void SemInit(sem_t* sem) {
  auto ok = sem_init(sem, 1, 0);
  if (ok == -1) {
    throw GetSyscallError();
  }
}

}  // namespace

template <typename MakeMapping>
MemMapping::MemMapping(MakeMapping mapping_maker, std::size_t length)
    : length_(length + sizeof(sem_t)), addr_(mapping_maker(length_)) {
  SemInit(static_cast<sem_t*>(static_cast<void*>(addr_)));
}

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

char* MakeAnonymousMapping(std::size_t length);

char* MakeFileMapping(std::size_t length);

char* MakePosixSharedMemory(std::size_t length);

}  // namespace stewkk::ipc
