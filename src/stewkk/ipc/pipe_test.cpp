#include <gmock/gmock.h>

#include <stewkk/ipc/fdstreambuf.hpp>
#include <stewkk/ipc/subprocess.hpp>
#include <stewkk/ipc/pipe.hpp>

using ::testing::Eq;

namespace stewkk::ipc {

void ChildProgramm(FDBufOut out) {
  out.sputn("hello", 5);
}

TEST(PipeTest, ReceivesMessageFromSubprocess) {
  Subprocess child(ChildProgramm, Pipe());
  std::string got;
  got.resize(5);

  child.stdout.sgetn(got.data(), got.size());

  ASSERT_THAT(got, Eq("hello"));
}

}  // namespace stewkk::ipc
