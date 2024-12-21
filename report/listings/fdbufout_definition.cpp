namespace stewkk::ipc {

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

std::streamsize FDBufOut::sputn(const char* buf, std::streamsize size) {
  auto res = write(fd_.value(), buf, size);
  return res;
}

}  // namespace stewkk::ipc
