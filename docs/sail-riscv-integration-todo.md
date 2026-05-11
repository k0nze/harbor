# Sail RISC-V Integration TODO

This checklist tracks the work needed to integrate the Sail RISC-V `c_emulator`
into Harbor and eventually connect its platform behavior to SystemC models.

## 1. Toolchain Setup

- [x] Add `external/sail` as the Sail compiler source dependency.
- [x] Add `external/sail-riscv` as the RISC-V Sail model dependency.
- [x] Add a Harbor CMake target for building the Sail compiler.
- [x] Choose the Harbor-local Sail installation prefix.
- [x] Add the selected Sail installation prefix to the documented setup flow.
- [x] Install the Sail compiler through the Harbor CMake target.
- [x] Verify the installed `sail` binary reports the expected version.

## 2. Build Sail RISC-V From Harbor

- [x] Reintroduce `external/sail-riscv` into the Harbor CMake build after the Sail compiler path is known.
- [x] Set `SAIL_BIN` to Harbor's installed Sail compiler.
- [x] Disable unnecessary Sail RISC-V test suites by default for normal Harbor development.
- [x] Add a Harbor-level `harbor_riscv_model` target that depends on Sail RISC-V's `riscv_model` library.
- [x] Build `harbor_riscv_model` from the Harbor repository.
- [x] Verify that `sail_riscv_model.cpp` and `sail_riscv_model.h` are generated in the Harbor build tree.

## 3. Minimal Harbor Wrapper

- [ ] Add a small public Harbor wrapper around Sail RISC-V's `ModelImpl`.
- [ ] Keep Sail RISC-V implementation details out of Harbor's public API where practical.
- [ ] Add initialization support for a JSON configuration string or file.
- [ ] Add minimal ELF loading support, either by reusing Sail RISC-V helper code or by wrapping it.
- [ ] Add a single-step API that calls the Sail model's step function.
- [ ] Add tests or a small example that initializes the model and executes a tiny ELF with an instruction limit.

## 4. Callback Observation

- [ ] Implement a Harbor callback class derived from Sail RISC-V's `callbacks_if`.
- [ ] Observe instruction fetches.
- [ ] Observe memory reads and writes.
- [ ] Observe PC writes and redirects.
- [ ] Observe CSR reads and writes.
- [ ] Observe traps and interrupts.
- [ ] Add an example trace callback that proves Harbor can monitor the emulator.

## 5. Platform And MMIO Bridge Design

- [ ] Identify the exact Sail RISC-V MMIO dispatch points in `model/sys/platform.sail` and `model/sys/mem.sail`.
- [ ] Decide which built-in devices should remain in Sail RISC-V initially, such as HTIF and CLINT.
- [ ] Define a Harbor peripheral bus interface for physical memory and MMIO access.
- [ ] Decide how failed external accesses map back to Sail memory operation results.
- [ ] Decide how SystemC time and Sail time should be synchronized.
- [ ] Decide how external interrupt state should be injected into the Sail model.

## 6. External MMIO Hook

- [ ] Add the smallest possible Sail RISC-V patch or extension point for external MMIO reads.
- [ ] Add the smallest possible Sail RISC-V patch or extension point for external MMIO writes.
- [ ] Route selected physical address ranges from Sail RISC-V to the Harbor peripheral bus.
- [ ] Preserve existing Sail RISC-V behavior for built-in devices and normal RAM.
- [ ] Add tests for successful external MMIO reads and writes.
- [ ] Add tests for failed external MMIO accesses and expected exception behavior.

## 7. SystemC Integration

- [ ] Add a SystemC-facing implementation of the Harbor peripheral bus interface.
- [ ] Map Sail RISC-V physical accesses to SystemC transactions.
- [ ] Add a minimal SystemC peripheral used by an emulator test program.
- [ ] Verify read/write ordering between the Sail emulator and SystemC model.
- [ ] Add a simple interrupt source from SystemC into the Sail model.
- [ ] Document the expected threading and scheduling assumptions.

## 8. Linux-Oriented Bring-Up

- [ ] Build the upstream Sail RISC-V OS boot example unchanged.
- [ ] Run the same image through Harbor's wrapper without SystemC peripherals.
- [ ] Replace one built-in platform component with a Harbor/SystemC-backed model.
- [ ] Add device-tree handling for Harbor-provided peripherals.
- [ ] Track Linux boot blockers as they appear.

## Open Design Questions

- [ ] Should Harbor maintain a small patch series against `external/sail-riscv`, or should it keep all changes outside the submodule?
- [ ] Should Harbor use Sail RISC-V's `ModelImpl` directly, or derive a Harbor-specific model implementation from `hart::Model`?
- [ ] Should RAM remain Sail runtime memory initially, or should all physical memory eventually be served through SystemC?
- [ ] Should the first SystemC interface use direct method calls or TLM sockets?
