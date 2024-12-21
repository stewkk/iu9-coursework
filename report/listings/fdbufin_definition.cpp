namespace stewkk::ipc {

FDBufIn::~FDBufIn() {
  if (fd_.has_value()) {
    Close(fd_.value());
  }
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

std::streamsize FDBufIn::sgetn(char* buf, std::streamsize size) {
  use_buf_ = false;
  return read(fd_.value(), buf, size);
}

}  // namespace stewkk::ipc
