# Harbor

## Setup

### macOS

```bash
brew install qemu
brew install --cask docker
```

Start Docker Desktop before building the examples.

Build the shared example container once:

```bash
docker build \
  -t harbor-examples-builder:latest \
  -f examples/Dockerfile \
  examples
```

All cross-compilation scripts use this image by default. Override it with
`HARBOR_EXAMPLES_DOCKER_IMAGE` if you build the image under a different tag.
The scripts keep compiler and Buildroot working state in Docker volumes and
copy final artifacts back into `build/`.

## RISC-V Assembly Bare-Metal Example

Build the example ELFs in Docker:

```bash
examples/riscv/minimal/build.sh
```

This minimal example is implemented in assembly only. The shared Docker image
provides the RISC-V cross compiler and CMake/Ninja build tooling.

Run it on QEMU:

```bash
examples/riscv/minimal/run.sh
```

Expected output:

```text
Hello from Harbor RISC-V bare metal on QEMU
```

## RISC-V C Bare-Metal Example

Build the example ELFs in Docker:

```bash
examples/riscv/hello/build.sh
```

The shared Docker image provides the RISC-V cross compiler and CMake/Ninja
build tooling.

Run it on QEMU:

```bash
examples/riscv/hello/run.sh
```

Expected output:

```text
Hello from Harbor RISC-V C on QEMU
```

## Integration Checks

Build and run the bare-metal RISC-V QEMU example checks:

```bash
tests/integration/run-riscv-examples.sh
```

The script builds the assembly and C ELFs in Docker, then runs both bare-metal
examples on QEMU.

Build and run all current integration checks, including the Buildroot Linux
baseline:

```bash
tests/integration/run-all.sh
```

The full integration script builds the bare-metal examples, builds the
Buildroot Linux baseline, and boots it until Linux starts `/init`. The Linux
boot log is written to `build/test-logs/`.

To run only the Linux boot check against existing Buildroot artifacts:

```bash
tests/integration/boot-buildroot-linux.sh
```

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

Terminate QEMU from the terminal with `Ctrl-a x`. Press `Ctrl-a h` in QEMU to
show the built-in shortcut help.

The fetch and build steps download external sources and can take a while.
Use the shared Harbor example container for the Buildroot fetch and build
steps. Native macOS is still useful for QEMU execution, but Buildroot setup
and compilation now run inside the container using Docker volumes for the
write-heavy source and output trees.

The Buildroot image expects guest networking. The QEMU run script attaches a
virtio network device backed by QEMU user-mode networking, and Buildroot brings
up `eth0` with DHCP during boot.
