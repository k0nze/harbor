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

When adding QEMU/SystemC integration, keep the first build targets small and
inspectable:

* one target to build Harbor libraries,
* one target to build a minimal example,
* one target to run a minimal smoke example if practical.

## SystemC/QEMU Integration Guidance

Prefer small, testable integration seams:

* Define Harbor-owned interfaces before binding directly to QEMU internals.
* Keep QEMU-specific adapter code separate from SystemC model code.
* Treat MMIO and memory transactions as the first integration path.
* Add timing annotation or synchronization only after functional transactions
  are working.
* Document assumptions around blocking behavior, threading, and time
  synchronization before relying on them.

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

