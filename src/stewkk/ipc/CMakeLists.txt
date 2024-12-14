add_library(libipc pipe.cpp subprocess.cpp errors.cpp fdstreambuf.cpp)
add_library(stewkk::libipc ALIAS libipc)
target_compile_features(libipc PUBLIC cxx_std_20)
set_target_properties(libipc PROPERTIES
  CXX_STANDART 20
  CXX_STANDART_REQUIRED YES
  CXX_EXTENSIONS YES
)
set_target_properties(libipc PROPERTIES OUTPUT_NAME stewkk_ipc)
target_include_directories(
  libipc PUBLIC $<INSTALL_INTERFACE:include>
                    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)
target_include_directories(
  libipc PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
)
target_compile_options(libipc PRIVATE ${BASE_COMPILE_FLAGS})

