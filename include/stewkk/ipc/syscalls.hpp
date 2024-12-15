#pragma once

#include <cstdlib>
#include <cstdint>
#include <filesystem>

namespace stewkk::ipc {

pid_t Fork();

void Close(std::int32_t fd);

void Waitpid(pid_t child_pid);

std::int32_t Open(std::filesystem::path path, std::int32_t flags);

}  // namespace stewkk::ipc
