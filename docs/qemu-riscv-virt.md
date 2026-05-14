# QEMU RISC-V `virt` Notes

Harbor starts with QEMU's RISC-V `virt` machine because it is the standard
generic QEMU platform for RISC-V software bring-up. It supports bare-metal
programs, OpenSBI, Linux, virtio devices, and generated device trees.

## Current Smoke Command

The current bare-metal smoke target runs:

```bash
qemu-system-riscv64 \
  -machine virt \
  -nographic \
  -bios none \
  -kernel build/examples/riscv/minimal/riscv-minimal.elf
```

`-bios none` is intentional for the bare-metal smoke test. QEMU loads the ELF
directly and starts at its entry point without firmware.

## Machine Assumptions Used By The Smoke Example

- RAM starts at `0x80000000`.
- The 16550-compatible UART is available at `0x10000000`.
- The QEMU RISC-V test finisher is available at `0x100000`.
- Writing `0x5555` to the test finisher exits QEMU successfully.

These addresses are used only by the current RISC-V `virt` smoke example. They
must not become generic Harbor/SystemC interface assumptions.

## Linux Boot Direction

The first Linux boot should use QEMU's `virt` machine as well.

Initial decisions:

- Use QEMU-provided OpenSBI first, rather than building custom firmware.
- Use a Linux kernel `Image` plus an initramfs or root filesystem first, rather
  than a combined firmware payload.
- Let QEMU provide the initial device tree first. Generate and inspect the DTB
  later when custom devices are introduced.
- Use Buildroot as the controlled baseline image source before evaluating real
  distro images.

This keeps the first Linux boot close to the standard QEMU flow and avoids
coupling Harbor to a custom firmware or device-tree strategy too early.

## Integration Boundary

Harbor's generic SystemC/TLM interfaces should not depend on the RISC-V `virt`
memory map. The QEMU RISC-V adapter is responsible for translating `virt`
machine details into Harbor-owned transaction, interrupt, reset, and timing
interfaces.

