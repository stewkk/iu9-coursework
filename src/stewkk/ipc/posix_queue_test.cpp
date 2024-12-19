#include <gmock/gmock.h>

#include <string>

#include <stewkk/ipc/fdstreambuf.hpp>
#include <stewkk/ipc/posix_queue.hpp>
#include <stewkk/ipc/subprocess.hpp>

using ::testing::Eq;
using std::literals::string_literals::operator""s;

namespace stewkk::ipc {

namespace {

void ChildProgramm(PosixQueueBufOut out) { out.sputn("hello", 5); }

}  // namespace

TEST(PosixQueueTest, ReceivesMessageFromSubprocess) {
  Subprocess<PosixQueueBufIn, PosixQueueBufOut> child(ChildProgramm,
                                                      PosixQueue("/stewkk-ipc-test"s, 5, 5));
  std::string got;
  got.resize(5);

  child.stdout.sgetn(got.data(), got.size());

  ASSERT_THAT(got, Eq("hello"));
}

}  // namespace stewkk::ipc
