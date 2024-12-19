#include <gmock/gmock.h>

#include <stewkk/ipc/fdstreambuf.hpp>
#include <stewkk/ipc/subprocess.hpp>
#include <stewkk/ipc/systemv_queue.hpp>

using ::testing::Eq;

namespace stewkk::ipc {

constexpr std::size_t kBufSize = 5;

namespace {

void ChildProgramm(SystemvQueueBufOut<kBufSize> out) { out.sputn("hello", 5); }

}  // namespace

TEST(SystemvQueueTest, ReceivesMessageFromSubprocess) {
  Subprocess<SystemvQueueBufIn<kBufSize>, SystemvQueueBufOut<kBufSize>> child(
      ChildProgramm, SystemvQueue<kBufSize>());
  std::string got;
  got.resize(5);

  child.stdout.sgetn(got.data(), got.size());

  ASSERT_THAT(got, Eq("hello"));
}

}  // namespace stewkk::ipc
