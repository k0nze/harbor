find_program(HARBOR_RISCV_GCC riscv64-elf-gcc)
find_program(HARBOR_QEMU_RISCV64 qemu-system-riscv64)

if(NOT HARBOR_RISCV_GCC)
  message(FATAL_ERROR "riscv64-elf-gcc not found. Install it with Homebrew: brew install riscv64-elf-gcc")
endif()

if(NOT HARBOR_QEMU_RISCV64)
  message(FATAL_ERROR "qemu-system-riscv64 not found. Install it with Homebrew: brew install qemu")
endif()

message(STATUS "Found RISC-V bare-metal GCC: ${HARBOR_RISCV_GCC}")
message(STATUS "Found QEMU RISC-V system emulator: ${HARBOR_QEMU_RISCV64}")

set(HARBOR_RISCV_MINIMAL_ELF "${CMAKE_CURRENT_BINARY_DIR}/riscv-minimal.elf")
set(HARBOR_RISCV_MINIMAL_MAP "${CMAKE_CURRENT_BINARY_DIR}/riscv-minimal.map")
set(HARBOR_RISCV_MINIMAL_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/linker.ld")

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

add_custom_target(harbor_riscv_minimal
  DEPENDS "${HARBOR_RISCV_MINIMAL_ELF}"
)

add_custom_target(harbor_run_riscv_minimal
  COMMAND
    "${HARBOR_QEMU_RISCV64}"
      -machine virt
      -nographic
      -bios none
      -kernel "${HARBOR_RISCV_MINIMAL_ELF}"
  DEPENDS harbor_riscv_minimal
  COMMENT "Running minimal RISC-V bare-metal ELF on QEMU"
  VERBATIM
)

