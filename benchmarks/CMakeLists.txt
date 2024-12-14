include(FetchGBenchmark)

add_executable(benchmarks main.cpp)
target_compile_features(benchmarks PRIVATE cxx_std_20)
set_target_properties(benchmarks PROPERTIES
  CXX_STANDART 20
  CXX_STANDART_REQUIRED YES
  CXX_EXTENSIONS YES
)
target_compile_options(benchmarks PRIVATE ${BASE_COMPILE_FLAGS})
target_link_libraries(benchmarks PRIVATE stewkk::libipc benchmark::benchmark)
