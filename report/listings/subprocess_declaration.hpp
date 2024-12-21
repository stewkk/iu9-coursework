#include <concepts>
#include <cstdlib>
#include <functional>
#include <streambuf>

namespace stewkk::ipc {

template <typename I>
concept StreamBuf = requires(I i) { std::derived_from<I, std::streambuf>; };

template <typename I>
concept RWIpc = requires(I i) {
  { i.GetReader() } -> StreamBuf;
  { i.GetWriter() } -> StreamBuf;
};

template <StreamBuf InBuf, StreamBuf OutBuf> class Subprocess {
public:
  template <RWIpc Ipc> Subprocess(std::function<void(OutBuf)> subprogram, Ipc ipc);
  ~Subprocess();
  Subprocess(const Subprocess& other) = delete;
  Subprocess& operator=(const Subprocess& other) = delete;

  InBuf stdout;

private:
  pid_t child_pid_;
};

}  // namespace stewkk::ipc
