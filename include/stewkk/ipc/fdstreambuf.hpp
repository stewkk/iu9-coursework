#pragma once

#include <cstdint>
#include <streambuf>
#include <optional>

namespace stewkk::ipc {

class FDBufOut : public std::streambuf {
public:
    explicit FDBufOut(std::int32_t fd);
    ~FDBufOut();
    FDBufOut(const FDBufOut& other) = delete;
    FDBufOut& operator=(const FDBufOut& other) = delete;
    FDBufOut(FDBufOut&& other) noexcept;
    FDBufOut& operator=(FDBufOut&& other) noexcept;

    std::streamsize sputn(const char* buf, std::streamsize size);

private:
    int_type overflow(int_type c) override;

private:
    std::optional<std::int32_t> fd_;
};

// NOTE: reference implementation
// https://github.com/pocoproject/poco/blob/b380b57d5d999444906e42754db13f87902a1794/Foundation/include/Poco/UnbufferedStreamBuf.h#L173
class FDBufIn : public std::streambuf {
public:
    explicit FDBufIn(std::int32_t fd);
    ~FDBufIn();
    FDBufIn(const FDBufIn& other) = delete;
    FDBufIn& operator=(const FDBufIn& other) = delete;
    FDBufIn(FDBufIn&& other) noexcept;
    FDBufIn& operator=(FDBufIn&& other) noexcept;

    std::streamsize sgetn(char* buf, std::streamsize size);

private:
    int_type uflow() override;
    int_type underflow() override;

private:
    std::optional<std::int32_t> fd_;
    char buf_;
    bool use_buf_;
};

}  // namespace stewkk::ipc
