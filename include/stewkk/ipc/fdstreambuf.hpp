#pragma once

#include <cstdint>
#include <streambuf>

namespace stewkk::ipc {

class FDBufOut : public std::streambuf {
public:
    explicit FDBufOut(std::int32_t fd);
    ~FDBufOut();

private:
    int_type overflow(int_type c) override;
    std::streamsize xsputn(const char* buf, std::streamsize size) override;

private:
    std::int32_t fd_;
};

// NOTE: reference implementation
// https://github.com/pocoproject/poco/blob/b380b57d5d999444906e42754db13f87902a1794/Foundation/include/Poco/UnbufferedStreamBuf.h#L173
class FDBufIn : public std::streambuf {
public:
    explicit FDBufIn(std::int32_t fd);
    ~FDBufIn();

private:
    int_type uflow() override;
    int_type underflow() override;
    std::streamsize xsgetn(char* buf, std::streamsize size) override;

private:
    std::int32_t fd_;
    char buf_;
    bool use_buf_;
};

}  // namespace stewkk::ipc
