#include <stewkk/ipc/fdstreambuf.hpp>

#include <unistd.h>

#include <iostream>
#include <utility>

#include <stewkk/ipc/errors.hpp>
#include <stewkk/ipc/syscalls.hpp>

namespace stewkk::ipc {

namespace {

bool WriteFD(std::int32_t fd, char data) {
    auto wrote = write(fd, &data, sizeof(data));

    if (wrote == -1) {
        throw GetSyscallError();
    }

    if (wrote != sizeof(data)) {
        return false;
    }

    return true;
}

template <std::size_t BufSize>
bool WriteFD(std::int32_t fd, std::array<std::int8_t, BufSize> data) {
    auto wrote = write(fd, data.data(), data.size());

    if (wrote == -1) {
        throw GetSyscallError();
    }

    if (wrote != data.size()) {
        return false;
    }

    return true;
}

}  // namespace

FDBufOut::FDBufOut(std::int32_t fd) : fd_(fd) {}

FDBufOut::~FDBufOut() {
    if (fd_.has_value()) {
        Close(fd_.value());
    }
}

FDBufOut::FDBufOut(FDBufOut&& other) noexcept : fd_(std::exchange(other.fd_, std::nullopt)) {}

FDBufOut& FDBufOut::operator=(FDBufOut&& other) noexcept {
    fd_ = std::exchange(other.fd_, std::nullopt);
    return *this;
}

FDBufOut::int_type FDBufOut::overflow(int_type c) {
    if (c == EOF) {
        return EOF;
    }

    auto ok = WriteFD(fd_.value(), c);
    if (!ok) {
        return EOF;
    }

    return c;
}

std::streamsize FDBufOut::xsputn(const char* buf, std::streamsize size) {
    auto res = write(fd_.value(), buf, size);
    return res;
}

FDBufIn::FDBufIn(std::int32_t fd) : fd_(fd), buf_(EOF), use_buf_(false) {}

FDBufIn::~FDBufIn() {
    if (fd_.has_value()) {
        Close(fd_.value());
    }
}

FDBufIn::FDBufIn(FDBufIn &&other) noexcept
    : fd_(std::exchange(other.fd_, std::nullopt)), buf_(other.buf_),
      use_buf_(other.use_buf_) {}

FDBufIn &FDBufIn::operator=(FDBufIn &&other) noexcept {
    fd_ = std::exchange(other.fd_, std::nullopt);
    buf_ = other.buf_;
    use_buf_ = other.use_buf_;
    return *this;
}

FDBufIn::int_type FDBufIn::underflow() {
    if (use_buf_) {
        return buf_;
    }

    char c;
    auto ok = read(fd_.value(), &c, sizeof(c));
    if (ok == -1) {
        return EOF;
    }
    if (ok != sizeof(c)) {
        return EOF;
    }
    use_buf_ = true;
    buf_ = c;
    return c;
}

FDBufIn::int_type FDBufIn::uflow() {
    if (use_buf_) {
        use_buf_ = false;
        return buf_;
    }

    char c;
    auto ok = read(fd_.value(), &c, sizeof(c));
    if (ok == -1) {
        return EOF;
    }
    if (ok != sizeof(c)) {
        return EOF;
    }
    return c;
}

std::streamsize FDBufIn::xsgetn(char* buf, std::streamsize size) {
    use_buf_ = false;
    return read(fd_.value(), buf, size);
}

}  // namespace stewkk::ipc
