#include <gmock/gmock.h>

#include <fcntl.h>

#include <stewkk/ipc/errors.hpp>

using ::testing::Eq;
using ::testing::ThrowsMessage;

namespace stewkk::ipc {

TEST(ErrorsTest, ThrowsSyscallError) {
  ASSERT_THAT(
      []() {
        if (open("this_file_not_exists", O_RDONLY) == -1) {
          throw GetSyscallError();
        }
      },
      ThrowsMessage<SyscallError>(Eq("ENOENT: No such file or directory")));
}

}  // namespace stewkk::ipc
