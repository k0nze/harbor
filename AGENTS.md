# Harbor

## Project Scope

Harbor is being reset toward fast virtual-platform exploration for RISC-V
systems. The intended direction is to connect a Linux-capable CPU emulator,
most likely QEMU, with SystemC/TLM components for peripherals, memory, cache,
interconnect, and approximate timing experiments.

The project is no longer centered on vendoring Sail or Sail RISC-V as
submodules. Do not reintroduce those submodules or Sail-specific build flows
unless explicitly requested.

## Current Design Bias

Prefer a pragmatic split of responsibilities:

* QEMU or another fast emulator runs Linux and provides practical software
  bring-up speed.
* SystemC/TLM models provide architectural exploration points such as MMIO
  devices, memory behavior, bus contention, and approximate timing.
* Timing estimates are acceptable; cycle-accurate CPU timing is not the first
  goal.
* Formal ISA reference models such as Sail may still be useful later for
  correctness checks, but should not drive the initial platform architecture.

## Architecture-Neutral Objective

Harbor's SystemC/TLM-facing interfaces should be architecture-neutral where
practical. Model generic concepts such as MMIO transactions, physical
addresses, access sizes, response status, interrupts, reset, and estimated
latency in Harbor-owned interfaces.

Keep architecture and machine details in dedicated adapters. RISC-V-specific
concepts such as OpenSBI, PLIC, CLINT, APLIC, IMSIC, and RISC-V `virt` memory
maps should not leak into generic SystemC interfaces. Arm-specific concepts
such as GIC, TF-A, EDK2, and Arm `virt` memory maps should be handled the same
way.

## Repository Structure

The repository is intentionally minimal after removing the previous Sail-based
prototype. When adding files, keep the structure explicit:

* `include/harbor`: public Harbor headers and interfaces.
* `src`: Harbor implementation files.
* `examples`: small integration examples and bring-up programs.
* `docs`: design notes, TODO lists, and architecture decisions.
* `external`: optional third-party sources only when intentionally vendored or
  added as submodules.

Generated directories such as `build/`, `toolchain/`, downloaded dependencies,
and emulator build trees should stay uncommitted.

## Build Direction

Prefer CMake as the top-level build entry point unless the project explicitly
moves to another build system.

Do not assume existing Sail targets such as `harbor_sail_compiler`,
`harbor_riscv_model`, or `sail_riscv_sim` still exist. They belonged to the
removed prototype.

When adding QEMU/SystemC integration, keep build targets small and inspectable:

* one target to build Harbor libraries,
* one target or explicit script to build the Harbor-enabled QEMU binary,
* one target to build a minimal example,
* run behavior should live in explicit scripts next to the examples or tests,
  not hidden behind CMake targets.

Packaged QEMU is acceptable for baseline Linux and bare-metal bring-up. For
Harbor-specific MMIO devices, QEMU should be added as an explicit submodule
under `external/qemu` and built from source natively on the host. Do not build
QEMU inside the examples Docker image.

Use `qemu-build.sh` for the initial host-native QEMU build. It should
build only `riscv64-softmmu` until Harbor needs additional targets. Prefer
putting `build/qemu/riscv64-softmmu` first in `PATH` and using the existing run
scripts instead of adding duplicate QEMU run wrappers.

## Docker-Based Cross Compilation

Cross compilation for guest artifacts should use the shared example builder
image defined by `examples/Dockerfile`.

The image can be prepared explicitly by the user or CI:

```bash
docker build \
  -t harbor-examples-builder:latest \
  -f examples/Dockerfile \
  examples
```

Do not make per-example build scripts create or update this image implicitly.
Those scripts should assume the prepared image exists and fail with a clear
message when it does not.

Integration scripts should call `tests/integration/ensure-examples-docker-image.sh`
before running Docker-based cross-compilation steps. This keeps regression runs
self-contained without hiding image creation inside per-example build scripts.

Use `HARBOR_EXAMPLES_DOCKER_IMAGE` when a workflow needs a different image tag.

Keep host and container responsibilities separate:

* Docker fetches/builds target artifacts such as RISC-V bare-metal ELFs,
  Buildroot sources, and Buildroot Linux images.
* The host builds and runs QEMU and other interactive or integration processes.
* Harbor core libraries should remain buildable on the host unless the project
  explicitly decides otherwise.

Use Docker volumes for write-heavy cross-compilation state instead of writing
large build trees through macOS bind mounts. Copy final artifacts back into
`build/` for host-side run scripts.

For RISC-V bare-metal examples, each example target should have its own build
script next to the example, such as `examples/riscv/minimal/build.sh` and
`examples/riscv/hello/build.sh`. Avoid reintroducing a single aggregate
`examples/riscv/build.sh` for all targets.

## Testing

Tests and integration checks are part of every major change. Run the relevant
checks before considering the task complete, and report any skipped checks with
the reason.

Current checks:

* `tests/integration/run-riscv-examples.sh` builds the current RISC-V QEMU
  assembly and C examples through their Docker-based per-example build scripts,
  then runs them on host QEMU.
* `tests/integration/run-all.sh` runs the complete current integration suite:
  the RISC-V bare-metal examples, the Buildroot Linux build, and the Linux boot
  check.
* `tests/integration/boot-buildroot-linux.sh` boots existing Buildroot
  artifacts, verifies that the root filesystem contains `mmio-test`, waits
  until Linux starts `/init`, `eth0` receives a DHCP lease, and the guest runs
  `mmio-test register-file --dry-run`, then terminates QEMU.
* `tests/cleanup.sh` removes generated build artifacts, downloaded Buildroot
  sources, and the prepared example builder image.

Use `tests/integration/run-riscv-examples.sh` for changes that affect CMake,
RISC-V examples, QEMU run scripts, toolchain detection, linker scripts, or
bare-metal runtime code.

The integration scripts prepare the shared example Docker image if it is
missing. If a change affects cross-compilation tooling or Linux bring-up, use
`tests/integration/run-all.sh` for full regression coverage.

## SystemC/QEMU Integration Guidance

Prefer small, testable integration seams:

* Define Harbor-owned interfaces before binding directly to QEMU internals.
* Keep QEMU-specific adapter code separate from SystemC model code.
* Treat MMIO and memory transactions as the first integration path.
* Use the minimal 16-entry, 32-bit register file documented in
  `docs/minimal-mmio-register-file.md` as the first guest-visible MMIO device.
* Add timing annotation or synchronization only after functional transactions
  are working.
* Document assumptions around blocking behavior, threading, and time
  synchronization before relying on them.

## Python

* Run Python always in a virtual enviroment.
* Install Python models always in a virtual enviroment.
* The Python virtual enviroment is under `.venv`

## Documentation

Public API comments should use Doxygen-compatible doc strings.

* Prefer documenting declarations in public headers under `include/harbor`
  instead of repeating the same documentation in source files.
* Document interface contracts and architectural behavior, not implementation
  trivia.
* Use Doxygen tags such as `@brief`, `@param`, `@return`, and `@note` when they
  add clarity.
* When a method has architectural constraints, timing assumptions, ownership
  requirements, or side effects, document them explicitly.
