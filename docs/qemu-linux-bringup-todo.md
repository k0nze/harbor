# QEMU Linux Bring-Up TODO

This checklist tracks the first Harbor steps after the reset toward a
QEMU-first virtual-platform flow. The goal is to move from local tool setup to
bare-metal examples, then to a Linux boot, before adding SystemC/TLM
integration.

## 1. Host Tool Setup

- [x] Decide whether Harbor uses the system QEMU package first or builds QEMU from source.
- [x] Document the required macOS packages for QEMU, RISC-V cross compilation, device trees, and build tooling.
- [x] Verify `qemu-system-riscv64` is available and record its version.
- [x] Verify the RISC-V bare-metal compiler is available and record its version.
- [x] Add a minimal top-level CMake project after the repository reset.
- [x] Add CMake detection for QEMU and the RISC-V bare-metal toolchain.

## 2. Repository Structure

- [x] Create `examples/riscv/minimal` for the first Harbor-owned RISC-V bare-metal program.
- [ ] Create `examples/linux` or `images/linux` for Linux boot helper scripts and generated image notes.
- [ ] Create `docs` notes for QEMU machine choices, boot commands, and integration decisions.
- [ ] Keep generated images, downloaded sources, and build artifacts out of Git.

## 3. Bare-Metal Examples On QEMU

- [x] Add a minimal RV64 bare-metal assembly program that exits through a QEMU-supported mechanism.
- [x] Add a linker script for the selected QEMU RISC-V machine memory map.
- [ ] Add a C runtime startup file for C bare-metal examples.
- [ ] Add a minimal C hello-world program.
- [x] Add CMake targets to build the bare-metal ELFs.
- [x] Add CMake or script targets to run the bare-metal ELFs on QEMU.
- [ ] Document equivalent direct compiler and QEMU commands for users of other build systems.
- [x] Record expected output and exit behavior.

## 4. QEMU Machine And Boot Contract

- [x] Choose the first QEMU machine, likely `virt`.
- [ ] Document the default RAM base, UART, CLINT, PLIC, and virtio devices for that machine.
- [ ] Decide whether the first Linux boot uses QEMU-provided OpenSBI or an explicit firmware image.
- [ ] Decide whether the first boot uses a kernel `Image`, an ELF, or a firmware payload.
- [ ] Decide how the initial device tree is provided, either QEMU-generated or Harbor-owned.

## 5. Linux Boot On QEMU

- [ ] Choose the first Linux image source, such as a prebuilt distro image, Buildroot, or a locally built kernel.
- [ ] Document the host tools needed to build or fetch that Linux image.
- [ ] Add a reproducible flow to obtain or build the kernel image.
- [ ] Add a reproducible flow to obtain or build a root filesystem.
- [ ] Add a QEMU command that boots Linux to a console.
- [ ] Add a shorter smoke boot command for quick validation.
- [ ] Record the expected boot log milestones, including OpenSBI, kernel start, rootfs mount, and shell or init.
- [ ] Document cleanup rules for generated images and downloads.

## 6. First SystemC/TLM Integration Plan

- [ ] Identify the smallest QEMU-to-Harbor boundary to prototype, such as a custom MMIO device.
- [ ] Decide whether to start with an in-process QEMU device, a remote-port style bridge, or another IPC bridge.
- [ ] Define a Harbor transaction interface for MMIO reads and writes.
- [ ] Add a trivial SystemC peripheral model that exposes one register.
- [ ] Route a Linux or bare-metal access to that register through QEMU into the SystemC model.
- [ ] Add basic timing annotation or counters only after functional access works.

## Open Design Questions

- [ ] Should Harbor depend on a packaged QEMU first, or vendor/build a specific QEMU revision?
- [ ] Should the first Linux image be Buildroot, Debian/Fedora, or a custom minimal initramfs?
- [ ] Should the first QEMU/SystemC bridge be in-process or process-separated?
- [ ] How should estimated time be synchronized between QEMU execution and SystemC simulation time?
- [ ] Which transactions should be timed first: MMIO, RAM, cache-line fills, or bus arbitration?
