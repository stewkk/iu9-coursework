#include <cstdint>

#include <stewkk/ipc/fdstreambuf.hpp>

namespace stewkk::ipc {

class Pipe {
public:
  using ReadFD = std::int32_t;
  using WriteFD = std::int32_t;

  Pipe();

  FDBufIn GetReader();
  FDBufOut GetWriter();

private:
  ReadFD read_fd_;
  WriteFD write_fd_;
};

}  // namespace stewkk::ipc
