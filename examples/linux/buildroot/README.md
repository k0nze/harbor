# Buildroot RISC-V QEMU `virt` Example

This example is Harbor's controlled Linux baseline for QEMU. It is intentionally
kept under `examples/linux` because Buildroot is one bring-up path, not Harbor's
main build system.

The flow uses a pinned Buildroot release and builds out-of-tree into `build/`.
Generated downloads, sources, images, and logs are not committed.

## Layout

```text
examples/linux/buildroot/
  buildroot.version
  riscv64_qemu_virt_defconfig
  fetch.sh
  build.sh
  run.sh
```

## Fetch Buildroot

```bash
examples/linux/buildroot/fetch.sh
```

This downloads and extracts Buildroot into `.buildroot/`.

## Build The Image

Use a Linux host or Linux container for this step. Native macOS currently
requires multiple Buildroot-internal workarounds and is not the verified path
for Harbor's Linux baseline.

```bash
examples/linux/buildroot/build.sh
```

This configures Buildroot with `riscv64_qemu_virt_defconfig` and builds into:

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
Buildroot kernel `Image`, and a CPIO initramfs for the root filesystem.

Expected boot milestones:

- QEMU starts OpenSBI.
- Linux prints its boot log on `ttyS0`.
- The initramfs is unpacked as the root filesystem.
- BusyBox init starts and provides the baseline userspace.

## Overrides

The scripts support these environment overrides:

- `BUILDROOT_VERSION`: Buildroot version to fetch.
- `BUILDROOT_SOURCE_DIR`: existing Buildroot source tree.
- `BUILDROOT_OUTPUT_DIR`: out-of-tree Buildroot output directory.
- `QEMU`: QEMU executable, defaults to `qemu-system-riscv64`.
- `MAKE`: GNU Make executable, defaults to `make`.
- `HOSTCC` and `HOSTCXX`: host compilers used by Buildroot.
- `BASH`: Bash executable used by Buildroot. It must support `mapfile`.
