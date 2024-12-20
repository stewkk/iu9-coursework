#include <stewkk/ipc/mmap.hpp>

#include <algorithm>
#include <utility>

#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>

#include <stewkk/ipc/errors.hpp>
#include <stewkk/ipc/syscalls.hpp>

namespace stewkk::ipc {

namespace {

char* MakeMapping(std::size_t length) {
  void* addr = mmap(nullptr, length, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if (addr == MAP_FAILED) {
    throw GetSyscallError();
  }
  return static_cast<char*>(addr);
}

void MemUnmap(char* addr, std::size_t length) {
  auto ok = munmap(addr, length);
  if (ok == -1) {
    throw GetSyscallError();
  }
}

void SemInit(sem_t* sem) {
  auto ok = sem_init(sem, 1, 0);
  if (ok == -1) {
    throw GetSyscallError();
  }
}

void SemDestroy(sem_t* sem) {
  auto ok = sem_destroy(sem);
  if (ok == -1) {
    throw GetSyscallError();
  }
}

void SemPost(sem_t* sem) {
  auto ok = sem_post(sem);
  if (ok == -1) {
    throw GetSyscallError();
  }
}

void SemWait(sem_t* sem) {
  auto ok = sem_wait(sem);
  if (ok == -1) {
    throw GetSyscallError();
  }
}

}  // namespace

MemMapping::MemMapping(std::size_t length)
    : length_(length + sizeof(sem_t)), addr_(MakeMapping(length_)) {
  SemInit(static_cast<sem_t*>(static_cast<void*>(addr_)));
}

MemMappingBufIn MemMapping::GetReader() { return MemMappingBufIn(length_, addr_); }

MemMappingBufOut MemMapping::GetWriter() { return MemMappingBufOut(length_, addr_); }

MemMappingBufOut::MemMappingBufOut(std::size_t length, char* addr)
    : length_(length), addr_(addr), cur_addr_(addr_ + sizeof(sem_t)) {}

MemMappingBufOut::~MemMappingBufOut() {
  if (addr_ != nullptr) {
    MemUnmap(addr_, length_);
  }
}

MemMappingBufOut::MemMappingBufOut(MemMappingBufOut&& other) noexcept
    : addr_(std::exchange(other.addr_, nullptr)),
      cur_addr_(std::exchange(other.cur_addr_, nullptr)),
      length_(other.length_) {}

MemMappingBufOut& MemMappingBufOut::operator=(MemMappingBufOut&& other) noexcept {
  addr_ = std::exchange(other.addr_, nullptr);
  cur_addr_ = std::exchange(other.cur_addr_, nullptr);
  length_ = other.length_;
  return *this;
}

MemMappingBufOut::int_type MemMappingBufOut::overflow(int_type c) { return EOF; }

std::streamsize MemMappingBufOut::sputn(const char* buf, std::streamsize size) {
  std::copy(buf, buf + size, cur_addr_);
  cur_addr_ += size;
  SemPost(static_cast<sem_t*>(static_cast<void*>(addr_)));
  return size;
}

MemMappingBufIn::MemMappingBufIn(std::size_t length, char* addr)
    : length_(length), addr_(addr), cur_addr_(addr_ + sizeof(sem_t)) {}

MemMappingBufIn::~MemMappingBufIn() {
  if (addr_ != nullptr) {
    SemDestroy(static_cast<sem_t*>(static_cast<void*>(addr_)));
    MemUnmap(addr_, length_);
  }
}

MemMappingBufIn::MemMappingBufIn(MemMappingBufIn&& other) noexcept
    : addr_(std::exchange(other.addr_, nullptr)),
      cur_addr_(std::exchange(other.cur_addr_, nullptr)),
      length_(other.length_) {}

MemMappingBufIn& MemMappingBufIn::operator=(MemMappingBufIn&& other) noexcept {
  addr_ = std::exchange(other.addr_, nullptr);
  cur_addr_ = std::exchange(other.cur_addr_, nullptr);
  length_ = other.length_;
  return *this;
}

MemMappingBufIn::int_type MemMappingBufIn::underflow() { return EOF; }

MemMappingBufIn::int_type MemMappingBufIn::uflow() { return EOF; }

std::streamsize MemMappingBufIn::sgetn(char* buf, std::streamsize size) {
  SemWait(static_cast<sem_t*>(static_cast<void*>(addr_)));
  std::copy(cur_addr_, cur_addr_ + size, buf);
  cur_addr_ += size;
  return size;
}

}  // namespace stewkk::ipc
