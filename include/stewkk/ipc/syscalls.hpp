#pragma once

#include <cstdlib>
#include <cstdint>

namespace stewkk::ipc {

pid_t Fork();

void Close(std::int32_t fd);

void Waitpid(pid_t child_pid);

}  // namespace stewkk::ipc
