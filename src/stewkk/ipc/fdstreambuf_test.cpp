#include <gmock/gmock.h>

#include <fcntl.h>
#include <fstream>

#include <experimental/scope>

#include <stewkk/ipc/fdstreambuf.hpp>

using ::testing::Eq;

namespace stewkk::ipc {

TEST(FdStreamBufTest, RdbufOutput) {
    {
      auto fd = open("/tmp/stewkk-ipc-fdbufout", O_CREAT | O_WRONLY, S_IRWXU);
      FDBufOut outbuf(fd);

      std::cout.exceptions(std::ios::badbit | std::ios::eofbit |
                           std::ios::eofbit);
      std::cin.exceptions(std::ios::badbit | std::ios::eofbit |
                          std::ios::eofbit);

      auto current = std::cout.rdbuf();
      std::experimental::scope_exit _(
          [&current]() { std::cout.rdbuf(current); });
      std::cout.rdbuf(&outbuf);

      std::cout << "hello" << std::endl;
    }

    std::string got;
    std::ifstream tmp("/tmp/stewkk-ipc-fdbufout");

    tmp >> got;

    ASSERT_THAT(got, Eq("hello"));
}

TEST(FdStreamBufTest, DirectOutput) {
    {
      auto fd = open("/tmp/stewkk-ipc-fdbufout", O_CREAT | O_WRONLY, S_IRWXU);
      FDBufOut outbuf(fd);

      std::string s = "hello";
      outbuf.sputn(s.data(), s.size());
    }

    std::string got;
    std::ifstream tmp("/tmp/stewkk-ipc-fdbufout");

    tmp >> got;

    ASSERT_THAT(got, Eq("hello"));
}

TEST(FdStreamBufTest, RdBufInput) {
  {
    std::ofstream tmp("/tmp/stewkk-ipc-fdbufin");
    tmp << "hello" << std::endl;
  }

  std::string got;
  {
    auto fd = open("/tmp/stewkk-ipc-fdbufout", O_RDONLY);
    FDBufIn inbuf(fd);
    auto current = std::cin.rdbuf();
    std::experimental::scope_exit _([&current]() { std::cin.rdbuf(current); });
    std::cin.rdbuf(&inbuf);

    std::cin >> got;
  }

  ASSERT_THAT(got, Eq("hello"));
}

TEST(FdStreamBufTest, DirectInput) {
  {
    std::ofstream tmp("/tmp/stewkk-ipc-fdbufin");
    tmp << "hello" << std::endl;
  }

  std::string got("bbbbb");
  {
    auto fd = open("/tmp/stewkk-ipc-fdbufout", O_RDONLY);
    FDBufIn inbuf(fd);

    inbuf.sgetn(got.data(), 5);
  }

  ASSERT_THAT(got, Eq("hello"));
}

}  // namespace stewkk::ipc
