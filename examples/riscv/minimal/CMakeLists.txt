find_program(HARBOR_RISCV_GCC
  NAMES
    riscv64-elf-gcc
    riscv64-unknown-elf-gcc
)

if(NOT HARBOR_RISCV_GCC)
  message(FATAL_ERROR "RISC-V bare-metal GCC not found. Use the Docker-based build scripts under examples/riscv/*/build.sh or install a riscv64 bare-metal toolchain locally.")
endif()

message(STATUS "Found RISC-V bare-metal GCC: ${HARBOR_RISCV_GCC}")

set(HARBOR_RISCV_MINIMAL_ELF "${CMAKE_CURRENT_BINARY_DIR}/riscv-minimal.elf")
set(HARBOR_RISCV_MINIMAL_MAP "${CMAKE_CURRENT_BINARY_DIR}/riscv-minimal.map")
set(HARBOR_RISCV_HELLO_ELF "${CMAKE_CURRENT_BINARY_DIR}/riscv-hello.elf")
set(HARBOR_RISCV_HELLO_MAP "${CMAKE_CURRENT_BINARY_DIR}/riscv-hello.map")
set(HARBOR_RISCV_SYSTEMC_MMIO_ELF "${CMAKE_CURRENT_BINARY_DIR}/riscv-systemc-mmio.elf")
set(HARBOR_RISCV_SYSTEMC_MMIO_MAP "${CMAKE_CURRENT_BINARY_DIR}/riscv-systemc-mmio.map")
set(HARBOR_RISCV_MINIMAL_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/linker.ld")
set(HARBOR_RISCV_COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../common")

add_custom_command(
  OUTPUT "${HARBOR_RISCV_MINIMAL_ELF}"
  COMMAND
    "${HARBOR_RISCV_GCC}"
      -march=rv64imac
      -mabi=lp64
      -nostdlib
      -nostartfiles
      "-T${HARBOR_RISCV_MINIMAL_LINKER_SCRIPT}"
      "-Wl,--no-relax"
      "-Wl,--no-warn-rwx-segments"
      "-Wl,-Map,${HARBOR_RISCV_MINIMAL_MAP}"
      -o "${HARBOR_RISCV_MINIMAL_ELF}"
      "${CMAKE_CURRENT_SOURCE_DIR}/start.S"
  DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/start.S"
    "${HARBOR_RISCV_MINIMAL_LINKER_SCRIPT}"
  COMMENT "Building minimal RISC-V bare-metal ELF for QEMU"
  VERBATIM
)

add_custom_target(harbor_riscv_assembly
  DEPENDS "${HARBOR_RISCV_MINIMAL_ELF}"
)

add_custom_command(
  OUTPUT "${HARBOR_RISCV_HELLO_ELF}"
  COMMAND
    "${HARBOR_RISCV_GCC}"
      -march=rv64imac
      -mabi=lp64
      -mcmodel=medany
      -ffreestanding
      -fno-builtin
      -Wall
      -Wextra
      -nostdlib
      -nostartfiles
      "-I${HARBOR_RISCV_COMMON_DIR}"
      "-T${HARBOR_RISCV_MINIMAL_LINKER_SCRIPT}"
      "-Wl,--no-relax"
      "-Wl,--no-warn-rwx-segments"
      "-Wl,-Map,${HARBOR_RISCV_HELLO_MAP}"
      -o "${HARBOR_RISCV_HELLO_ELF}"
      "${HARBOR_RISCV_COMMON_DIR}/start.S"
      "${HARBOR_RISCV_COMMON_DIR}/runtime.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/../hello/main.c"
  DEPENDS
    "${HARBOR_RISCV_COMMON_DIR}/start.S"
    "${HARBOR_RISCV_COMMON_DIR}/runtime.c"
    "${HARBOR_RISCV_COMMON_DIR}/runtime.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../hello/main.c"
    "${HARBOR_RISCV_MINIMAL_LINKER_SCRIPT}"
  COMMENT "Building RISC-V C hello ELF for QEMU"
  VERBATIM
)

add_custom_target(harbor_riscv_c
  DEPENDS "${HARBOR_RISCV_HELLO_ELF}"
)

add_custom_command(
  OUTPUT "${HARBOR_RISCV_SYSTEMC_MMIO_ELF}"
  COMMAND
    "${HARBOR_RISCV_GCC}"
      -march=rv64imac
      -mabi=lp64
      -mcmodel=medany
      -ffreestanding
      -fno-builtin
      -Wall
      -Wextra
      -nostdlib
      -nostartfiles
      "-I${HARBOR_RISCV_COMMON_DIR}"
      "-T${HARBOR_RISCV_MINIMAL_LINKER_SCRIPT}"
      "-Wl,--no-relax"
      "-Wl,--no-warn-rwx-segments"
      "-Wl,-Map,${HARBOR_RISCV_SYSTEMC_MMIO_MAP}"
      -o "${HARBOR_RISCV_SYSTEMC_MMIO_ELF}"
      "${HARBOR_RISCV_COMMON_DIR}/start.S"
      "${HARBOR_RISCV_COMMON_DIR}/runtime.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/../systemc-mmio/main.c"
  DEPENDS
    "${HARBOR_RISCV_COMMON_DIR}/start.S"
    "${HARBOR_RISCV_COMMON_DIR}/runtime.c"
    "${HARBOR_RISCV_COMMON_DIR}/runtime.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../systemc-mmio/main.c"
    "${HARBOR_RISCV_MINIMAL_LINKER_SCRIPT}"
  COMMENT "Building RISC-V SystemC MMIO ELF for QEMU"
  VERBATIM
)

add_custom_target(harbor_riscv_systemc_mmio
  DEPENDS "${HARBOR_RISCV_SYSTEMC_MMIO_ELF}"
)
