#include <gmock/gmock.h>

#include <stewkk/ipc/pipe.hpp>

using ::testing::StartsWith;

TEST(ExampleTest, Example) {
  auto s = "Hello World!";
  EXPECT_THAT(s, StartsWith("Hello"));
}
