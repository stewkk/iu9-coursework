#include <stewkk/ipc/mmap.hpp>

#include <algorithm>
#include <utility>
#include <vector>

#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>

#include <stewkk/ipc/errors.hpp>
#include <stewkk/ipc/syscalls.hpp>

namespace stewkk::ipc {

namespace {

void MemUnmap(char* addr, std::size_t length) {
  auto ok = munmap(addr, length);
  if (ok == -1) {
    throw GetSyscallError();
  }
}

void MemSync(char* addr, size_t length) {
  auto ok = msync(addr, length, MS_SYNC);
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

std::int32_t ShmOpen(std::string name) {
  auto fd = shm_open(name.c_str(), O_CREAT | O_RDWR | O_TRUNC, S_IRWXU);
  if (fd == -1) {
    throw GetSyscallError();
  }
  return fd;
}

void FileTruncate(std::int32_t fd, std::size_t length) {
  auto ok = ftruncate(fd, length);
  if (ok == -1) {
    throw GetSyscallError();
  }
}

}  // namespace

MemMappingBufIn MemMapping::GetReader() { return MemMappingBufIn(length_, addr_); }

MemMappingBufOut MemMapping::GetWriter() { return MemMappingBufOut(length_, addr_); }

MemMappingBufOut::MemMappingBufOut(std::size_t length, char* addr)
    : length_(length), addr_(addr), cur_addr_(addr_ + sizeof(sem_t)) {
  auto ok = madvise(addr_, length, MADV_SEQUENTIAL);
  if (ok == -1) {
    throw GetSyscallError();
  }
}

MemMappingBufOut::~MemMappingBufOut() {
  if (addr_ != nullptr) {
    MemSync(addr_, length_);
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
    : length_(length), addr_(addr), cur_addr_(addr_ + sizeof(sem_t)) {
  auto ok = madvise(addr, length, MADV_SEQUENTIAL);
  if (ok == -1) {
    throw GetSyscallError();
  }
}

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

char* MakeAnonymousMapping(std::size_t length) {
  void* addr = mmap(nullptr, length, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if (addr == MAP_FAILED) {
    throw GetSyscallError();
  }
  return static_cast<char*>(addr);
}

char* MakeFileMapping(std::size_t length) {
  auto fd = open("/tmp/stewkk-ipc-mmap", O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
  if (fd == -1) {
    throw GetSyscallError();
  }

  FileTruncate(fd, length);

  void* addr = mmap(nullptr, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED) {
    throw GetSyscallError();
  }

  Close(fd);

  return static_cast<char*>(addr);
}

char* MakePosixSharedMemory(std::size_t length) {
  auto fd = ShmOpen("/stewkk-ipc-shm");
  FileTruncate(fd, length);

  void* addr = mmap(nullptr, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED) {
    throw GetSyscallError();
  }

  Close(fd);

  return static_cast<char*>(addr);
}

}  // namespace stewkk::ipc
