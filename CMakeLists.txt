cmake_minimum_required(VERSION 3.20)

project(harbor
  VERSION 0.1.0
  LANGUAGES CXX
)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

option(HARBOR_BUILD_RISCV_EXAMPLES "Build RISC-V example targets" ON)
option(HARBOR_BUILD_TESTS "Build Harbor unit tests" ON)

add_library(harbor
  src/mmio/register_file.cpp
)

target_compile_features(harbor
  PUBLIC cxx_std_17
)

target_include_directories(harbor
  PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

add_library(harbor_qemu_adapter STATIC
  src/qemu/mmio_adapter.cpp
)

set_target_properties(harbor_qemu_adapter
  PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

target_compile_features(harbor_qemu_adapter
  PUBLIC cxx_std_17
)

target_link_libraries(harbor_qemu_adapter
  PUBLIC harbor
)

if(HARBOR_BUILD_TESTS)
  enable_testing()

  add_executable(harbor_register_file_test
    tests/unit/register_file_test.cpp
  )

  target_link_libraries(harbor_register_file_test
    PRIVATE harbor
  )

  add_test(
    NAME harbor_register_file_test
    COMMAND harbor_register_file_test
  )
endif()

if(HARBOR_BUILD_RISCV_EXAMPLES)
  add_subdirectory(examples/riscv/minimal)
endif()
