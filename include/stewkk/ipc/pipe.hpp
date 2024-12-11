#pragma once

#include <array>
#include <cstdio>
#include <cstdint>
#include <unistd.h>

namespace stewkk::ipc {

template <std::size_t BufSize>
class PipeReader;

template <std::size_t BufSize>
class PipeWriter;

class Pipe {
public:
    using ReadFD = std::int32_t;
    using WriteFD = std::int32_t;

    Pipe();

    template <std::size_t BufSize>
    PipeReader<BufSize> GetReader();

    template <std::size_t BufSize>
    PipeWriter<BufSize> GetWriter();
private:
    ReadFD read_fd_;
    WriteFD write_fd_;
};

template <std::size_t BufSize>
PipeReader<BufSize> Pipe::GetReader() {
    close(write_fd_);
    return PipeReader<BufSize>(read_fd_);
}

template <std::size_t BufSize>
PipeWriter<BufSize> Pipe::GetWriter() {
    close(read_fd_);
    return PipeWriter<BufSize>(write_fd_);
}

template <std::size_t BufSize>
class PipeReader {
public:
    PipeReader() = delete;

    std::array<std::int8_t, BufSize> Read();
private:
    Pipe::ReadFD read_fd_;
};

template <std::size_t BufSize>
std::array<std::int8_t, BufSize> PipeReader<BufSize>::Read() {

}

template <std::size_t BufSize>
class PipeWriter {
public:
    PipeWriter() = delete;

    std::array<std::int8_t, BufSize> Write();
private:
    Pipe::WriteFD write_fd_;
};

template <std::size_t BufSize>
std::array<std::int8_t, BufSize> PipeWriter<BufSize>::Write() {

}

}  // namespace stewkk::ipc
