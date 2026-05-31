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
option(HARBOR_ENABLE_SYSTEMC "Enable host SystemC integration targets" ON)

if(HARBOR_ENABLE_SYSTEMC)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(HARBOR_SYSTEMC REQUIRED IMPORTED_TARGET systemc)

  add_library(harbor_systemc_dependency INTERFACE)
  target_link_libraries(harbor_systemc_dependency
    INTERFACE PkgConfig::HARBOR_SYSTEMC
  )
endif()

add_library(harbor
  src/mmio/transaction.cpp
)

target_compile_features(harbor
  PUBLIC cxx_std_17
)

target_include_directories(harbor
  PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(HARBOR_ENABLE_SYSTEMC)
  add_library(harbor_systemc
    src/systemc/runtime.cpp
    src/systemc/single_register.cpp
  )

  target_compile_features(harbor_systemc
    PUBLIC cxx_std_17
  )

  target_include_directories(harbor_systemc
    PUBLIC
      "${CMAKE_CURRENT_SOURCE_DIR}/include"
  )

  target_link_libraries(harbor_systemc
    PUBLIC
      harbor
      harbor_systemc_dependency
  )
endif()

if(HARBOR_ENABLE_SYSTEMC)
  add_library(harbor_qemu_adapter STATIC
    src/qemu/systemc_mmio_adapter.cpp
  )

  set_target_properties(harbor_qemu_adapter
    PROPERTIES
      POSITION_INDEPENDENT_CODE ON
  )

  target_compile_features(harbor_qemu_adapter
    PUBLIC cxx_std_17
  )

  target_link_libraries(harbor_qemu_adapter
    PUBLIC
      harbor_systemc
  )
endif()

if(HARBOR_BUILD_TESTS)
  enable_testing()

  add_executable(harbor_mmio_transaction_test
    tests/unit/mmio_transaction_test.cpp
  )

  target_link_libraries(harbor_mmio_transaction_test
    PRIVATE harbor
  )

  add_test(
    NAME harbor_mmio_transaction_test
    COMMAND harbor_mmio_transaction_test
  )

  if(HARBOR_ENABLE_SYSTEMC)
    add_executable(harbor_systemc_single_register_test
      tests/unit/systemc_single_register_test.cpp
    )

    target_link_libraries(harbor_systemc_single_register_test
      PRIVATE harbor_systemc
    )

    add_test(
      NAME harbor_systemc_single_register_test
      COMMAND harbor_systemc_single_register_test
    )

    add_executable(harbor_qemu_systemc_adapter_test
      tests/unit/qemu_systemc_adapter_test.cpp
    )

    target_link_libraries(harbor_qemu_systemc_adapter_test
      PRIVATE harbor_qemu_adapter
    )

    add_test(
      NAME harbor_qemu_systemc_adapter_test
      COMMAND harbor_qemu_systemc_adapter_test
    )
  endif()
endif()

if(HARBOR_BUILD_RISCV_EXAMPLES)
  add_subdirectory(examples/riscv/minimal)
endif()
