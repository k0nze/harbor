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
- [x] Create `docs` notes for QEMU machine choices, boot commands, and integration decisions.
- [x] Keep generated images, downloaded sources, and build artifacts out of Git.

## 3. Bare-Metal Examples On QEMU

- [x] Add a minimal RV64 bare-metal assembly program that exits through a QEMU-supported mechanism.
- [x] Add a linker script for the selected QEMU RISC-V machine memory map.
- [x] Add a C runtime startup file for C bare-metal examples.
- [x] Add a minimal C hello-world program.
- [x] Add CMake targets to build the bare-metal ELFs.
- [x] Add scripts to run the bare-metal ELFs on QEMU.
- [x] Add an integration script that builds and runs the current RISC-V examples.
- [x] Add a full integration script that also builds and boots the Buildroot Linux baseline.
- [x] Document equivalent direct compiler and QEMU commands for users of other build systems.
- [x] Record expected output and exit behavior.

## 4. QEMU Machine And Boot Contract

- [x] Choose the first QEMU machine, likely `virt`.
- [x] Document the default RAM base, UART, CLINT, PLIC, and virtio devices for that machine.
- [x] Decide whether the first Linux boot uses QEMU-provided OpenSBI or an explicit firmware image.
- [x] Decide whether the first boot uses a kernel `Image`, an ELF, or a firmware payload.
- [x] Decide how the initial device tree is provided, either QEMU-generated or Harbor-owned.

## 5. Linux Phase 1: Buildroot Baseline

- [x] Use Buildroot as the first controlled Linux image source.
- [x] Document the host tools needed to build Buildroot images.
- [x] Add a reproducible flow to obtain or build a minimal RV64 Buildroot image.
- [x] Build a small kernel, BusyBox userspace, and initramfs or root filesystem.
- [x] Add a QEMU command that boots the Buildroot image to a console.
- [x] Add a shorter Linux boot command for quick validation.
- [x] Record the expected boot log milestones, including OpenSBI, kernel start, rootfs mount, and shell or init.
- [x] Add a small userspace test program that accesses the Harbor SystemC MMIO device.
- [x] Run the SystemC MMIO test automatically during the Linux boot integration check and print each MMIO read/write on the command line.
- [x] Document cleanup rules for generated images, downloads, Docker volumes, and Harbor-installed QEMU integration artifacts.

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

## 8. First Harbor/QEMU MMIO Integration

- [x] Identify the smallest QEMU-to-Harbor boundary to prototype: a custom MMIO device backed by a Harbor C++ model.
- [x] Define the first Harbor-side MMIO test model: a SystemC-backed 32-bit register.
- [x] Decide to start with a custom QEMU build so Harbor can add a QEMU-visible MMIO device.
- [x] Add upstream QEMU as a Git submodule under `external/qemu`.
- [x] Add host-native QEMU build scripts or CMake integration that build only the required system targets first.
- [x] Keep QEMU build artifacts out of Git and outside Docker; Docker remains only for guest cross-compilation.
- [x] Decide that the first QEMU/Harbor connection is direct in-process through a thin QEMU C shim and Harbor-owned C ABI adapter.
- [x] Define the first Harbor register access interface through the environment-neutral MMIO transaction interface and `include/harbor/qemu/mmio_adapter.h`.
- [x] Add a QEMU-visible MMIO mapping for the SystemC-backed register.
- [x] Add Linux integration coverage that reads the SystemC reset value, writes the register, and reads it back.

## 9. First SystemC/TLM Integration Plan

- [x] Add SystemC to the documented host dependencies.
- [x] Add CMake detection for the host SystemC installation.
- [x] Add a small Harbor SystemC library target that is separate from QEMU-owned source files.
- [x] Define the Harbor-owned MMIO transaction boundary that QEMU adapters call without exposing QEMU internals to SystemC models.
- [x] Add a trivial SystemC peripheral model that exposes one 32-bit register.
- [x] Add a C++ adapter that connects the existing QEMU-facing Harbor C ABI to the SystemC-backed register model.
- [x] Add host-side tests for the SystemC register model and adapter before using QEMU.
- [x] Route a Linux or bare-metal access to that register through QEMU into the SystemC model.
- [x] Add integration coverage that proves the Linux guest can read/write the SystemC-backed register.
- [ ] Add basic timing annotation or counters only after functional access works.
- [ ] Define how QEMU consumes Harbor MMIO latency annotations.
- [ ] Define the QEMU/SystemC time synchronization policy.
- [ ] Replace the one-register proof-of-concept with a reusable SystemC/TLM peripheral pattern.
- [ ] Add a real TLM socket and transport path behind the Harbor MMIO transaction boundary.
- [ ] Extend the SystemC path beyond one register to cover multiple registers and devices.
- [ ] Define reset, interrupt, and lifecycle interfaces for SystemC models.
- [ ] Document QEMU/SystemC threading, blocking behavior, and ownership assumptions.
- [ ] Add SystemC integration tests for invalid accesses, multiple devices, timing annotations, and reset behavior.
- [ ] Add SystemC-backed models for the next exploration targets: memory behavior, interrupt lines, DMA-capable devices, bus or interconnect behavior, and cache or memory timing.

## Open Design Questions

- [x] Should Harbor depend on a packaged QEMU first, or vendor/build a specific QEMU revision?
  Decision: packaged QEMU remains useful for baseline bring-up, but Harbor-specific MMIO integration requires QEMU as a submodule and a host-native QEMU build.
- [ ] Which real distro should Harbor evaluate first after the Buildroot baseline?
- [ ] Does Harbor need Yocto at all, or are Buildroot plus real distro images sufficient?
- [ ] Should the first QEMU/SystemC bridge be in-process or process-separated?
- [ ] How should estimated time be synchronized between QEMU execution and SystemC simulation time?
- [ ] Which transactions should be timed first: MMIO, RAM, cache-line fills, or bus arbitration?
