#include <gmock/gmock.h>

#include <stewkk/ipc/fdstreambuf.hpp>
#include <stewkk/ipc/mmap.hpp>
#include <stewkk/ipc/subprocess.hpp>

using ::testing::Eq;

namespace stewkk::ipc {

namespace {

void ChildProgramm(MemMappingBufOut out) { out.sputn("hello", 5); }

}  // namespace

TEST(MmapTest, AnonymousMapping) {
  Subprocess<MemMappingBufIn, MemMappingBufOut> child(ChildProgramm,
                                                      MemMapping(MakeAnonymousMapping, 5));
  std::string got;
  got.resize(5);

  child.stdout.sgetn(got.data(), got.size());

  ASSERT_THAT(got, Eq("hello"));
}

TEST(MmapTest, FileMapping) {
  Subprocess<MemMappingBufIn, MemMappingBufOut> child(ChildProgramm,
                                                      MemMapping(MakeFileMapping, 5));
  std::string got;
  got.resize(5);

  child.stdout.sgetn(got.data(), got.size());

  ASSERT_THAT(got, Eq("hello"));
}

}  // namespace stewkk::ipc
