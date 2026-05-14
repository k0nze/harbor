# Harbor

## Setup

### macOS

```bash
brew install \
  cmake \
  ninja \
  qemu \
  systemc \
  riscv64-elf-gcc \
  dtc \
  make \
  gnu-tar \
  curl \
  xz
```

## RISC-V Assembly Bare-Metal Example

Configure Harbor:

```bash
cmake -B build -G Ninja
```

This smoke example is implemented in assembly only. It does not compile any C
source code. The build still uses `riscv64-elf-gcc` as the assembler/linker
driver for the guest ELF.

Build the RISC-V assembly ELF:

```bash
cmake --build build --target harbor_riscv_assembly
```

Equivalent direct compile command:

```bash
riscv64-elf-gcc \
  -march=rv64imac \
  -mabi=lp64 \
  -nostdlib \
  -nostartfiles \
  -Texamples/riscv/minimal/linker.ld \
  -Wl,--no-relax \
  -Wl,--no-warn-rwx-segments \
  -Wl,-Map,build/examples/riscv/minimal/riscv-minimal.map \
  -o build/examples/riscv/minimal/riscv-minimal.elf \
  examples/riscv/minimal/start.S
```

Run it on QEMU:

```bash
examples/riscv/minimal/run.sh
```

Expected output:

```text
Hello from Harbor RISC-V bare metal on QEMU
```

## RISC-V C Bare-Metal Example

Build the RISC-V C hello ELF:

```bash
cmake --build build --target harbor_riscv_c
```

Equivalent direct compile command:

```bash
riscv64-elf-gcc \
  -march=rv64imac \
  -mabi=lp64 \
  -mcmodel=medany \
  -ffreestanding \
  -fno-builtin \
  -Wall \
  -Wextra \
  -nostdlib \
  -nostartfiles \
  -Iexamples/riscv/common \
  -Texamples/riscv/minimal/linker.ld \
  -Wl,--no-relax \
  -Wl,--no-warn-rwx-segments \
  -Wl,-Map,build/examples/riscv/minimal/riscv-hello.map \
  -o build/examples/riscv/minimal/riscv-hello.elf \
  examples/riscv/common/start.S \
  examples/riscv/common/runtime.c \
  examples/riscv/hello/main.c
```

Run it on QEMU:

```bash
examples/riscv/hello/run.sh
```

Expected output:

```text
Hello from Harbor RISC-V C on QEMU
```
