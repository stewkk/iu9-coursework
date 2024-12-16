include(FetchGTest)
include(GoogleTest)

add_executable(unittests)
target_sources(unittests PRIVATE
  ${PROJECT_SOURCE_DIR}/src/stewkk/ipc/pipe_test.cpp
  ${PROJECT_SOURCE_DIR}/src/stewkk/ipc/fifo_test.cpp
  ${PROJECT_SOURCE_DIR}/src/stewkk/ipc/socket_test.cpp
  ${PROJECT_SOURCE_DIR}/src/stewkk/ipc/fdstreambuf_test.cpp
  ${PROJECT_SOURCE_DIR}/src/stewkk/ipc/errors_test.cpp
)
target_compile_options(unittests PRIVATE ${BASE_COMPILE_FLAGS})
target_link_libraries(unittests PRIVATE stewkk::libipc gmock_main)
gtest_discover_tests(unittests)
