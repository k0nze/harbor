cmake_minimum_required(VERSION 3.20)

project(harbor
  VERSION 0.1.0
  LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(HARBOR_SAIL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/sail")
set(HARBOR_SAIL_RISCV_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/sail-riscv")
set(HARBOR_SAIL_INSTALL_PREFIX "" CACHE PATH "Installation prefix for the Sail compiler built by Harbor")

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

add_custom_target(harbor_sail_compiler
  COMMAND "${OPAM_BIN}" exec -- dune build --release
  WORKING_DIRECTORY "${HARBOR_SAIL_SOURCE_DIR}"
  COMMENT "Building the Sail compiler"
  VERBATIM
)

if(HARBOR_SAIL_INSTALL_PREFIX)
  add_custom_target(harbor_sail_install
    COMMAND "${OPAM_BIN}" exec -- dune install --prefix "${HARBOR_SAIL_INSTALL_PREFIX}"
    WORKING_DIRECTORY "${HARBOR_SAIL_SOURCE_DIR}"
    COMMENT "Installing the Sail compiler to ${HARBOR_SAIL_INSTALL_PREFIX}"
    VERBATIM
  )
  add_dependencies(harbor_sail_install harbor_sail_compiler)
else()
  add_custom_target(harbor_sail_install
    COMMAND "${CMAKE_COMMAND}" -E false
    COMMENT "Set -DHARBOR_SAIL_INSTALL_PREFIX=<path> before installing the Sail compiler"
    VERBATIM
  )
endif()
