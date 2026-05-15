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
  bash \
  gcc@12 \
  coreutils \
  dtc \
  findutils \
  gpatch \
  util-linux \
  make \
  gnu-tar \
  curl \
  xz
```

Use Homebrew's GNU tools before the default macOS tools:

```bash
export PATH="$(brew --prefix make)/libexec/gnubin:$PATH"
export PATH="$(brew --prefix bash)/bin:$PATH"
export PATH="$(brew --prefix coreutils)/libexec/gnubin:$PATH"
export PATH="$(brew --prefix findutils)/libexec/gnubin:$PATH"
export PATH="$(brew --prefix gpatch)/libexec/gnubin:$PATH"
export PATH="$(brew --prefix util-linux)/bin:$PATH"
export PATH="$(brew --prefix util-linux)/libexec/gnubin:$PATH"
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

## Integration Checks

Build and run all current RISC-V QEMU examples:

```bash
tests/integration/run-riscv-examples.sh
```

The script configures CMake, builds the assembly and C ELFs, and runs both
examples on QEMU.

## Buildroot Linux Baseline

The first controlled Linux baseline for QEMU `virt` lives under:

```text
examples/linux/buildroot
```

Start with:

```bash
examples/linux/buildroot/fetch.sh
examples/linux/buildroot/build.sh
examples/linux/buildroot/run.sh
```

The fetch and build steps download external sources and can take a while.
Use a Linux host or Linux container for the Buildroot build. Native macOS is
useful for the QEMU bare-metal examples, but Buildroot 2025.02 still assumes
Linux host conventions in parts of its autotools package flow.
