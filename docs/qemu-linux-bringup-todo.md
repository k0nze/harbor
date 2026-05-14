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
- [x] Create `examples/linux` or `images/linux` for Linux boot helper scripts and generated image notes.
- [ ] Create `docs` notes for QEMU machine choices, boot commands, and integration decisions.
- [x] Keep generated images, downloaded sources, and build artifacts out of Git.

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

## 5. Linux Phase 1: Buildroot Baseline

- [ ] Use Buildroot as the first controlled Linux image source.
- [ ] Document the host tools needed to build Buildroot images.
- [ ] Add a reproducible flow to obtain or build a minimal RV64 Buildroot image.
- [ ] Build a small kernel, BusyBox userspace, and initramfs or root filesystem.
- [ ] Add a QEMU command that boots the Buildroot image to a console.
- [ ] Add a shorter smoke boot command for quick validation.
- [ ] Record the expected boot log milestones, including OpenSBI, kernel start, rootfs mount, and shell or init.
- [ ] Add a small userspace test program that can later access a Harbor-provided MMIO device.
- [ ] Document cleanup rules for generated images and downloads.

## 6. Linux Phase 2: Real Distro Evaluation

- [ ] Choose the first real distro image to evaluate, such as Debian, Fedora, Ubuntu, or Alpine.
- [ ] Document how to obtain or build that distro image for RISC-V and, later, Arm.
- [ ] Add a QEMU command that boots the distro image to a login shell or serial console.
- [ ] Document storage, root filesystem, and kernel command-line assumptions.
- [ ] Verify common userspace services or workloads relevant to peripheral evaluation.
- [ ] Reuse the same future Harbor/QEMU peripheral path that was validated with Buildroot.
- [ ] Record boot time, image size, and runtime overhead compared with the Buildroot baseline.

## 7. Linux Phase 3: Optional Yocto/Product-Style Images

- [ ] Decide whether Harbor needs Yocto after Buildroot and real distro flows are working.
- [ ] If needed, choose a Yocto release and board/machine configuration.
- [ ] Document the Yocto layer strategy for Harbor-specific devices or tests.
- [ ] Add a reproducible Yocto image build flow only if product-style image generation is required.
- [ ] Keep Yocto optional so it does not block fast QEMU/SystemC iteration.

## 8. First SystemC/TLM Integration Plan

- [ ] Identify the smallest QEMU-to-Harbor boundary to prototype, such as a custom MMIO device.
- [ ] Decide whether to start with an in-process QEMU device, a remote-port style bridge, or another IPC bridge.
- [ ] Define a Harbor transaction interface for MMIO reads and writes.
- [ ] Add a trivial SystemC peripheral model that exposes one register.
- [ ] Route a Linux or bare-metal access to that register through QEMU into the SystemC model.
- [ ] Add basic timing annotation or counters only after functional access works.

## Open Design Questions

- [ ] Should Harbor depend on a packaged QEMU first, or vendor/build a specific QEMU revision?
- [ ] Which real distro should Harbor evaluate first after the Buildroot baseline?
- [ ] Does Harbor need Yocto at all, or are Buildroot plus real distro images sufficient?
- [ ] Should the first QEMU/SystemC bridge be in-process or process-separated?
- [ ] How should estimated time be synchronized between QEMU execution and SystemC simulation time?
- [ ] Which transactions should be timed first: MMIO, RAM, cache-line fills, or bus arbitration?
