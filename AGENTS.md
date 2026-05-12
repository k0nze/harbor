# Harbor

## Project Scope

The goal of this project is to connect the `c_emulator` of the formal RISC-V ISA definition under `external/sail-riscv` to a SystemC hardware interface. The Sail RISC-V model owns ISA decode, execution semantics, architectural state, privilege behavior, and CSR behavior. Harbor should provide the integration layer around that model.

The intended direction is that the Sail RISC-V `c_emulator` boots a Linux kernel while selected platform resources, peripherals, timing, interrupts, memory, or cache behavior can be provided through SystemC models in this repository.

Do not reintroduce a separate hand-written RISC-V decoder or instruction semantics layer in Harbor unless explicitly requested. The previous standalone instruction-set-simulator direction has been superseded by the Sail RISC-V integration direction.

## External Dependencies

* `external/sail` contains the Sail compiler source dependency
* `external/sail-riscv` contains the RISC-V formal model and C emulator support code
* Treat both external directories as upstream dependencies
* Avoid modifying files inside external submodules unless the task explicitly requires a patch against the upstream project
* Generated files and build outputs inside external submodules should not be committed unless explicitly intended

## Build

Harbor uses CMake as the top-level build entry point.

* `harbor_sail_compiler` builds the Sail compiler from `external/sail`
* `harbor_sail_install` installs the Sail compiler into Harbor's local toolchain prefix
* `harbor_riscv_model` builds Sail RISC-V's generated model and `riscv_model` library from Harbor
* `harbor_baremetal_minimal` builds the first Harbor-owned RV64GC bare-metal ELF
* `harbor_run_baremetal_minimal` builds and runs that ELF with Sail RISC-V's `sail_riscv_sim`
* The default Sail install prefix is `toolchain/sail`
* `toolchain/` is generated local output and should stay uncommitted

The known working OCaml setup is:

* OCaml `4.14.2`
* Menhir `20240715`
* an active opam switch named `harbor`

The bare-metal example flow expects the Homebrew RISC-V ELF toolchain on macOS:

* `riscv64-elf-gcc`
* `riscv64-elf-binutils`

Follow `README.md` for exact setup commands. Keep the README updated when build requirements change.

## Repository Structure

* Harbor-owned bare-metal examples live under `examples/baremetal`
* Keep example programs, linker scripts, and C runtime startup code outside `external/sail-riscv`
* Prefer CMake targets for reproducible local flows, but document equivalent direct compiler/linker commands when useful for other build systems
* The minimal bare-metal example exits through Sail RISC-V's HTIF `tohost` mechanism

## Integration Direction

The next integration milestones are tracked in `docs/sail-riscv-integration-todo.md`.

Prefer this order of work:

* build and install the Sail compiler from Harbor
* build Sail RISC-V's `riscv_model` from Harbor using the installed Sail compiler
* get familiar with Sail RISC-V's `c_emulator` through small Harbor-owned bare-metal examples
* document the emulator command-line options, ELF loading behavior, entry-point behavior, and HTIF exit behavior used by the examples
* add a minimal Harbor wrapper around Sail RISC-V's `ModelImpl` only after the standalone emulator flow is understood
* add observation callbacks using Sail RISC-V's `callbacks_if`
* design and implement the SystemC/MMIO bridge
* avoid touching the Sail RISC-V platform model until the callback and wrapper path is understood

## Documentation

* Public API comments should use Doxygen-compatible doc strings
* Prefer documenting declarations in public headers under `include/harbor` instead of repeating the same documentation in source files
* Document interface contracts and architectural behavior, not implementation trivia
* Use Doxygen tags such as `@brief`, `@param`, `@return`, and `@note` when they add clarity
* When a method has architectural constraints or side effects, document them explicitly
