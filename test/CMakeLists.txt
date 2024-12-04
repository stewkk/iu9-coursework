include(FetchGTest)
include(GoogleTest)

add_executable(unittests ${PROJECT_SOURCE_DIR}/src/stewkk/ipc/pipe_test.cpp)
target_compile_options(unittests PRIVATE ${BASE_COMPILE_FLAGS})
target_include_directories(
  unittests PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
)
target_link_libraries(unittests PRIVATE stewkk::libipc gmock_main)
gtest_discover_tests(unittests)
