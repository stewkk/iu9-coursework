#include <stewkk/ipc/subprocess.hpp>

#include <functional>

namespace stewkk::ipc {

Subprocess::Subprocess(std::function<void()> programm) {}

void Subprocess::Wait() {}


}  // namespace stewkk::ipc
