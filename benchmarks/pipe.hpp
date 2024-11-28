#pragma once

#include <cstring>
#include <iostream>
#include <cstdint>
#include <array>

using std::string_literals::operator""s;

#include <unistd.h>
#include <wait.h>

constexpr std::size_t kBufSize = 10;

inline auto LogSyscallError() -> void {
   auto err = errno;
   std::cerr << strerrorname_np(err) << ": " << strerror(err) << std::endl;
}

inline auto Pipe() -> int {
    std::array<std::int32_t, 2> pipe_file_descriptors;
    std::array<char, kBufSize> buffer;
    if (auto err = pipe(pipe_file_descriptors.data()); err == -1) {
        LogSyscallError();
        return 1;
    }

    if (auto child = fork(); child == -1) {
        LogSyscallError();
        return 1;
    } else if (child == 0) {
        if (close(pipe_file_descriptors[1]) == -1) {
            LogSyscallError();
            return 1;
        }

        while (true) {
            auto num_read = read(pipe_file_descriptors[0], buffer.data(), kBufSize);
            if (num_read == -1) {
                LogSyscallError();
                return 1;
            }

            if (num_read == 0) {
                break;
            }
        }

        if (close(pipe_file_descriptors[0]) == -1) {
            LogSyscallError();
            return 1;
        }

        exit(0);
    } else {
        if (close(pipe_file_descriptors[0]) == -1) {
            LogSyscallError();
            return 1;
        }

        auto s =  "hello world"s;
        if (write(pipe_file_descriptors[1], s.data(), s.size()) != s.size()) {
            std::cerr << "write failed" << std::endl;
            return 1;
        }

        if (close(pipe_file_descriptors[1]) == -1) {
            LogSyscallError();
            return 1;
        }

        wait(nullptr);
    }

    return 0;
}
