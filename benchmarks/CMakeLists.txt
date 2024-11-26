add_executable(main main.cpp)
target_compile_features(main PRIVATE cxx_std_20)
set_target_properties(main PROPERTIES
  CXX_STANDART 20
  CXX_STANDART_REQUIRED YES
  CXX_EXTENSIONS YES
)
target_compile_options(main PRIVATE ${BASE_COMPILE_FLAGS})
