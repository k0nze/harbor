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

## Minimal RISC-V Bare-Metal Example

Configure Harbor:

```bash
cmake -B build -G Ninja
```

The minimal QEMU smoke target does not require a host C or C++ compiler. It
uses `riscv64-elf-gcc` directly for the guest ELF.

Build the minimal RISC-V ELF:

```bash
cmake --build build --target harbor_riscv_minimal
```

Run it on QEMU:

```bash
cmake --build build --target harbor_run_riscv_minimal
```

Expected output:

```text
Hello from Harbor RISC-V bare metal on QEMU
```
