#include <gmock/gmock.h>

#include <stewkk/ipc/fdstreambuf.hpp>
#include <stewkk/ipc/socket_pair.hpp>
#include <stewkk/ipc/subprocess.hpp>

using ::testing::Eq;

namespace stewkk::ipc {

namespace {

void ChildProgramm(FDBufOut out) { out.sputn("hello", 5); }

}  // namespace

TEST(SocketPairTest, StreamSocketEcho) {
  Subprocess<FDBufIn, FDBufOut> child(ChildProgramm, SocketPair(SocketPair::Type::kStreamSocket));
  std::string got;
  got.resize(5);

  child.stdout.sgetn(got.data(), got.size());

  ASSERT_THAT(got, Eq("hello"));
}

TEST(SocketPairTest, DatagramSocketEcho) {
  Subprocess<FDBufIn, FDBufOut> child(ChildProgramm, SocketPair(SocketPair::Type::kDatagramSocket));
  std::string got;
  got.resize(5);

  child.stdout.sgetn(got.data(), got.size());

  ASSERT_THAT(got, Eq("hello"));
}

}  // namespace stewkk::ipc
