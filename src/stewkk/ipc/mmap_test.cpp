#include <gmock/gmock.h>

#include <stewkk/ipc/fdstreambuf.hpp>
#include <stewkk/ipc/mmap.hpp>
#include <stewkk/ipc/subprocess.hpp>

using ::testing::Eq;

namespace stewkk::ipc {

namespace {

void ChildProgramm(MemMappingBufOut out) { out.sputn("hello", 5); }

}  // namespace

TEST(MmapTest, ReceivesMessageFromSubprocess) {
  Subprocess<MemMappingBufIn, MemMappingBufOut> child(ChildProgramm, MemMapping(5));
  std::string got;
  got.resize(5);

  child.stdout.sgetn(got.data(), got.size());

  ASSERT_THAT(got, Eq("hello"));
}

}  // namespace stewkk::ipc
