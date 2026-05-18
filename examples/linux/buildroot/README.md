# Buildroot RISC-V QEMU `virt` Example

This example is Harbor's controlled Linux baseline for QEMU. It is intentionally
kept under `examples/linux` because Buildroot is one bring-up path, not Harbor's
main build system.

The flow uses a pinned Buildroot release. Downloads, extracted sources, and
Buildroot's out-of-tree output live in Docker volumes; final boot artifacts are
copied back into `build/`. Generated files are not committed.

## Layout

```text
examples/linux/buildroot/
  buildroot.version
  riscv64_qemu_virt_defconfig
  external/
  fetch.sh
  build.sh
  run.sh
```

The `external/` directory is a Harbor-owned Buildroot external tree. It carries
packages that should be installed into the test image without modifying
Buildroot itself.

## Fetch Buildroot

Use the shared Harbor example container documented in the top-level README for
this step.

```bash
examples/linux/buildroot/fetch.sh
```

This downloads and extracts Buildroot into the `harbor-buildroot-source` Docker
volume.

## Build The Image

Use the same shared Harbor example container for this step. It provides the
Buildroot host tools and the RISC-V bare-metal toolchain in one image.

```bash
examples/linux/buildroot/build.sh
```

This configures Buildroot with `riscv64_qemu_virt_defconfig` and builds inside
the `harbor-buildroot-output` Docker volume. Final artifacts are copied to:

```text
build/buildroot/riscv64-qemu-virt
```

Expected output artifacts:

```text
build/buildroot/riscv64-qemu-virt/images/Image
build/buildroot/riscv64-qemu-virt/images/rootfs.cpio
```

## Run On QEMU

```bash
examples/linux/buildroot/run.sh
```

The run script uses QEMU's RISC-V `virt` machine, QEMU-provided OpenSBI, the
Buildroot kernel `Image`, a CPIO initramfs for the root filesystem, and QEMU
user-mode networking through a virtio network device. Buildroot configures
`eth0` with DHCP during boot.

Expected boot milestones:

- QEMU starts OpenSBI.
- Linux prints its boot log on `ttyS0`.
- The initramfs is unpacked as the root filesystem.
- `eth0` is configured through QEMU user-mode DHCP.
- BusyBox init starts and provides the baseline userspace.

## MMIO Test Utility

The image installs `mmio-test`, a small userspace utility for future validation
of Harbor-provided MMIO devices. It can parse and dry-run a physical MMIO
access before a device exists:

```sh
mmio-test --help
mmio-test register-file --dry-run
```

The package also installs `/etc/init.d/S90mmio-test`, which runs the dry-run
during boot. The integration test checks for this output on the serial console.

Once a QEMU/SystemC MMIO device is available, the same utility can be used from
inside the guest:

```sh
mmio-test read 0x10010000
mmio-test write 0x10010000 0x12345678
mmio-test register-file check
```

The default access width is 32 bits. Pass `8`, `16`, `32`, or `64` as the final
argument to select a different width.

## Overrides

The scripts support these environment overrides:

- `BUILDROOT_VERSION`: Buildroot version to fetch.
- `BUILDROOT_SOURCE_DIR`: existing Buildroot source tree inside the container.
- `BUILDROOT_OUTPUT_DIR`: host directory where final artifacts are copied.
- `HARBOR_BUILDROOT_SOURCE_VOLUME`: Docker volume for downloads and sources.
- `HARBOR_BUILDROOT_OUTPUT_VOLUME`: Docker volume for Buildroot output.
- `QEMU`: QEMU executable, defaults to `qemu-system-riscv64`.
- `MAKE`: GNU Make executable, defaults to `make`.
- `HOSTCC` and `HOSTCXX`: host compilers used by Buildroot.
- `BASH`: Bash executable used by Buildroot. It must support `mapfile`.
