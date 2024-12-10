#include <stewkk/ipc/pipe.hpp>

#include <array>
#include <cstdint>
#include <utility>

#include <unistd.h>

namespace stewkk::ipc {

using PipeReadFD = std::int32_t;
using PipeWriteFD = std::int32_t;

std::pair<PipeReadFD, PipeWriteFD> MakePipe() {
    std::array<std::int32_t, 2> pipe_file_descriptors;
    if (pipe(pipe_file_descriptors.data()) == -1) {
        throw std::exception();
    }
    return {
        pipe_file_descriptors[0],
        pipe_file_descriptors[1],
    };
}

}  // namespace stewkk::ipc
