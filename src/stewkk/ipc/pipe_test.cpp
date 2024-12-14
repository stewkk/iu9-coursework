#include <gmock/gmock.h>

#include <stewkk/ipc/pipe.hpp>

using ::testing::Eq;

namespace stewkk::ipc {

TEST(PipeTest, Simple) {
  Pipe();
}

}  // namespace stewkk::ipc
