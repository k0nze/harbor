cmake_minimum_required(VERSION 3.20)

project(harbor
  VERSION 0.1.0
  LANGUAGES C CXX
)

include(ExternalProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

set(HARBOR_SAIL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/sail")
set(HARBOR_SAIL_RISCV_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/sail-riscv")
set(HARBOR_DEFAULT_SAIL_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/toolchain/sail")
set(HARBOR_SAIL_INSTALL_PREFIX "${HARBOR_DEFAULT_SAIL_INSTALL_PREFIX}" CACHE PATH "Installation prefix for the Sail compiler built by Harbor")

if(NOT HARBOR_SAIL_INSTALL_PREFIX)
  set(HARBOR_SAIL_INSTALL_PREFIX "${HARBOR_DEFAULT_SAIL_INSTALL_PREFIX}" CACHE PATH "Installation prefix for the Sail compiler built by Harbor" FORCE)
endif()

find_program(OPAM_BIN opam)

if(NOT OPAM_BIN)
  message(FATAL_ERROR "opam not found. Install Sail's OCaml dependencies before building the Sail compiler.")
endif()

if(NOT EXISTS "${HARBOR_SAIL_SOURCE_DIR}/dune-project")
  message(FATAL_ERROR "Sail source directory not found at ${HARBOR_SAIL_SOURCE_DIR}")
endif()

if(NOT EXISTS "${HARBOR_SAIL_RISCV_SOURCE_DIR}/CMakeLists.txt")
  message(FATAL_ERROR "sail-riscv source directory not found at ${HARBOR_SAIL_RISCV_SOURCE_DIR}")
endif()

set(HARBOR_SAIL_BIN "${HARBOR_SAIL_INSTALL_PREFIX}/bin/sail" CACHE FILEPATH "Sail compiler used to build sail-riscv")

if(NOT EXISTS "${HARBOR_SAIL_BIN}")
  message(FATAL_ERROR "Sail compiler not found at ${HARBOR_SAIL_BIN}. Build and install it with the harbor_sail_install target.")
endif()

add_custom_target(harbor_sail_compiler
  COMMAND "${OPAM_BIN}" exec -- dune build --release
  WORKING_DIRECTORY "${HARBOR_SAIL_SOURCE_DIR}"
  COMMENT "Building the Sail compiler"
  VERBATIM
)

add_custom_target(harbor_sail_install
  COMMAND "${OPAM_BIN}" exec -- dune install --prefix "${HARBOR_SAIL_INSTALL_PREFIX}"
  WORKING_DIRECTORY "${HARBOR_SAIL_SOURCE_DIR}"
  COMMENT "Installing the Sail compiler to ${HARBOR_SAIL_INSTALL_PREFIX}"
  VERBATIM
)
add_dependencies(harbor_sail_install harbor_sail_compiler)

set(HARBOR_SAIL_RISCV_BINARY_DIR "${CMAKE_BINARY_DIR}/sail-riscv")

ExternalProject_Add(harbor_sail_riscv
  SOURCE_DIR "${HARBOR_SAIL_RISCV_SOURCE_DIR}"
  BINARY_DIR "${HARBOR_SAIL_RISCV_BINARY_DIR}"
  INSTALL_COMMAND ""
  CONFIGURE_COMMAND
    "${CMAKE_COMMAND}" -E env --unset=CC --unset=CXX
    "${CMAKE_COMMAND}"
      -S "${HARBOR_SAIL_RISCV_SOURCE_DIR}"
      -B "${HARBOR_SAIL_RISCV_BINARY_DIR}"
      -G "${CMAKE_GENERATOR}"
      "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
      "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
      "-DSAIL_BIN=${HARBOR_SAIL_BIN}"
      "-DENABLE_RISCV_TESTS=OFF"
      "-DENABLE_RISCV_ARCH_TESTS=OFF"
      "-DFIRST_PARTY_TESTS=OFF"
      "-DDOWNLOAD_GMP=OFF"
  BUILD_COMMAND
    "${CMAKE_COMMAND}" -E env --unset=CC --unset=CXX
    "${CMAKE_COMMAND}" --build "${HARBOR_SAIL_RISCV_BINARY_DIR}" --target riscv_model
)

add_custom_target(harbor_riscv_model DEPENDS harbor_sail_riscv)
