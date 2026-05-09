# RVISS

## Project Scope

The goal of this project is to connect the `c_emulator` of the fromal RISC-V ISA definition under `external/sail-riscv` to a SystemC hardware interface. E.g. the `c_emulator` boots a linux kernel and the registers, cache, and memory is provided through SystemC models in this repository.

## Documentation

* Public API comments should use Doxygen-compatible doc strings
* Prefer documenting declarations in public headers under `include/rviss` instead of repeating the same documentation in source files
* Document interface contracts and architectural behavior, not implementation trivia
* Use Doxygen tags such as `@brief`, `@param`, `@return`, and `@note` when they add clarity
* When a method has architectural constraints or side effects, document them explicitly


