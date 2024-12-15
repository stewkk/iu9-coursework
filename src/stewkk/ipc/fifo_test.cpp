#include <gmock/gmock.h>

#include <filesystem>

#include <stewkk/ipc/fdstreambuf.hpp>
#include <stewkk/ipc/subprocess.hpp>
#include <stewkk/ipc/fifo.hpp>

using ::testing::Eq;

namespace stewkk::ipc {

namespace {

void ChildProgramm(FDBufOut out) {
  out.sputn("hello", 5);
}

}  // namespace

TEST(FifoTest, ReceivesMessageFromSubprocess) {
  auto path = std::filesystem::temp_directory_path() / "coursework-fifo-test";
  std::filesystem::remove(path);
  Subprocess child(ChildProgramm, Fifo(path));
  std::string got;
  got.resize(5);

  child.stdout.sgetn(got.data(), got.size());

  ASSERT_THAT(got, Eq("hello"));
}

}  // namespace stewkk::ipc
