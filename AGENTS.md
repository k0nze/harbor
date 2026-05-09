# RVISS

## Project Scope

This project is intended to build a purely functional RISC-V instruction set simulator in C++ 17 to learn more about RISC-V and eventually be able to boot a Linux kernel while the performance is a secondary goal.

In the first step of the implementation floating point should not be used.

## Structure

* `src` contains the implementation
* `docs` contains useful information and tracks the project status
* `include` contains the public headers
* `include/rviss` contains the public library interface
* `include/rviss/rviss.h` is the umbrella header intended for apps
* `include/rviss/isa` contains ISA-related public headers
* `tests` contains the tests
* `tests/unit` contains Catch2-based unit tests
* `apps` contains examples of how to use the instruction set simulator
* `build` contains the build output generated with `cmake` and `ninja`

## Core And State Separation

* `include/rviss/state` and `src/state` contain architectural state interfaces and implementations
* `state` is for persistent machine state that instructions observe or modify, such as registers, PC, CSRs, memory access primitives, trap state, and atomic reservation state
* `include/rviss/core` and `src/core` contain simulator engine logic
* `core` is for fetch, decode, dispatch, stepping, and other orchestration that operates on an architectural state but is not itself architectural state
* If a component represents machine state, place it under `state`
* If a component drives or interprets execution using that state, place it under `core`

## Documentation

* Public API comments should use Doxygen-compatible doc strings
* Prefer documenting declarations in public headers under `include/rviss` instead of repeating the same documentation in source files
* Document interface contracts and architectural behavior, not implementation trivia
* Use Doxygen tags such as `@brief`, `@param`, `@return`, and `@note` when they add clarity
* When a method has architectural constraints or side effects, document them explicitly

## Build

From the project root:

* `cmake -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON`
* `cmake --build build`

## Testing

The project uses Catch2 for unit tests and CTest for test execution.

From the project root:

* `cmake -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON`
* `cmake --build build`
* `ctest --test-dir build --output-on-failure`
