#include <gmock/gmock.h>

#include <chrono>

#include <stewkk/ipc/subprocess.hpp>

using ::testing::Eq;

namespace stewkk::ipc {

void ChildProgramm() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

TEST(SubprocessTest, ExecutesStdFunction) {
  ASSERT_NO_THROW({
    Subprocess child(ChildProgramm);
    child.Wait();
  });
}

}  // namespace stewkk::ipc
