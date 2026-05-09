#ifndef RVISS_STATE_ARCHITECTURAL_STATE_H
#define RVISS_STATE_ARCHITECTURAL_STATE_H

#include "rviss/isa/register.h"
#include <cstddef>
#include <cstdint>

/**
 * @brief Abstract interface for the architecturally visible state of a RISC-V hart.
 *
 * This interface is the boundary between instruction semantics and a concrete
 * simulation backend. Implementations provide the persistent machine state that
 * decoded instructions observe and modify, while higher-level simulator
 * components in `core` drive fetch, decode, and execution using this interface.
 *
 * The intent is to keep instruction behavior independent from a particular
 * runtime environment so the same semantic layer can be reused in standalone
 * simulations and integrations such as SystemC or SST.
 *
 * ArchitecturalState models the state of a single hart. It exposes:
 * - integer register access
 * - program counter access
 * - instruction fetch
 * - data memory access
 * - CSR access
 * - load-reserved/store-conditional primitives
 * - exception and interrupt signaling
 * - XLEN reporting
 *
 * @note Implementations should preserve architectural invariants explicitly.
 * In particular, writes to register `x0` should behave as a no-op and reads of
 * `x0` should always yield zero.
 *
 * @note The methods in this interface represent architecturally visible effects.
 * They are not required to reflect any specific internal storage layout.
 */
class ArchitecturalState {
public:
    /**
     * @brief Destroy the architectural state interface.
     */
    virtual ~ArchitecturalState() = 0;

    /**
     * @brief Read an integer general-purpose register.
     *
     * @param reg Architectural register identifier.
     * @return The current XLEN-wide value of the register, zero-extended to
     * `uint64_t` when necessary.
     */
    virtual uint64_t read_gpr(Register reg) const = 0;

    /**
     * @brief Write an integer general-purpose register.
     *
     * @param reg Architectural register identifier.
     * @param value Value to be written to the register.
     *
     * @note Implementations should preserve the RISC-V `x0` invariant. A write
     * to `Register::zero` should not modify state.
     */
    virtual void write_gpr(Register reg, uint64_t value) = 0;

    /**
     * @brief Read the current program counter.
     *
     * @return The architecturally visible program counter value.
     */
    virtual uint64_t read_pc() const = 0;

    /**
     * @brief Write the current program counter.
     *
     * @param value New architecturally visible program counter value.
     */
    virtual void write_pc(uint64_t value) = 0;

    /**
     * @brief Fetch a 16-bit instruction parcel from memory.
     *
     * @param address Byte address of the instruction fetch.
     * @return The 16-bit value fetched from the instruction stream.
     *
     * @note Fetch is intentionally separate from data loads because instruction
     * access may eventually require different fault behavior, permissions, or
     * accounting than ordinary data memory reads.
     */
    virtual uint16_t fetch16(uint64_t address) const = 0;

    /**
     * @brief Fetch a 32-bit instruction word from memory.
     *
     * @param address Byte address of the instruction fetch.
     * @return The 32-bit value fetched from the instruction stream.
     *
     * @note For compressed instruction handling, higher-level fetch/decode
     * logic may still choose to inspect only the low 16 bits of a 32-bit word.
     */
    virtual uint32_t fetch32(uint64_t address) const = 0;

    /**
     * @brief Load an 8-bit value from data memory.
     *
     * @param address Byte address of the memory access.
     * @return The loaded 8-bit value.
     */
    virtual uint8_t load8(uint64_t address) const = 0;

    /**
     * @brief Load a 16-bit value from data memory.
     *
     * @param address Byte address of the memory access.
     * @return The loaded 16-bit value.
     */
    virtual uint16_t load16(uint64_t address) const = 0;

    /**
     * @brief Load a 32-bit value from data memory.
     *
     * @param address Byte address of the memory access.
     * @return The loaded 32-bit value.
     */
    virtual uint32_t load32(uint64_t address) const = 0;

    /**
     * @brief Load a 64-bit value from data memory.
     *
     * @param address Byte address of the memory access.
     * @return The loaded 64-bit value.
     */
    virtual uint64_t load64(uint64_t address) const = 0;

    /**
     * @brief Store an 8-bit value to data memory.
     *
     * @param address Byte address of the memory access.
     * @param value Value to store.
     */
    virtual void store8(uint64_t address, uint8_t value) = 0;

    /**
     * @brief Store a 16-bit value to data memory.
     *
     * @param address Byte address of the memory access.
     * @param value Value to store.
     */
    virtual void store16(uint64_t address, uint16_t value) = 0;

    /**
     * @brief Store a 32-bit value to data memory.
     *
     * @param address Byte address of the memory access.
     * @param value Value to store.
     */
    virtual void store32(uint64_t address, uint32_t value) = 0;

    /**
     * @brief Store a 64-bit value to data memory.
     *
     * @param address Byte address of the memory access.
     * @param value Value to store.
     */
    virtual void store64(uint64_t address, uint64_t value) = 0;

    /**
     * @brief Read a control and status register.
     *
     * @param csr Architectural CSR number.
     * @return The current XLEN-wide CSR value, zero-extended to `uint64_t`
     * when necessary.
     */
    virtual uint64_t read_csr(uint16_t csr) const = 0;

    /**
     * @brief Write a control and status register.
     *
     * @param csr Architectural CSR number.
     * @param value Value to be written.
     */
    virtual void write_csr(uint16_t csr, uint64_t value) = 0;

    /**
     * @brief Perform a 32-bit load-reserved operation.
     *
     * @param address Byte address of the reservation and load.
     * @return The loaded 32-bit value.
     *
     * @note Implementations should establish or update the reservation state so
     * that a subsequent `store_conditional32` can determine whether the
     * reservation is still valid.
     */
    virtual uint32_t load_reserved32(uint64_t address) = 0;

    /**
     * @brief Perform a 64-bit load-reserved operation.
     *
     * @param address Byte address of the reservation and load.
     * @return The loaded 64-bit value.
     *
     * @note Implementations should establish or update the reservation state so
     * that a subsequent `store_conditional64` can determine whether the
     * reservation is still valid.
     */
    virtual uint64_t load_reserved64(uint64_t address) = 0;

    /**
     * @brief Perform a 32-bit store-conditional operation.
     *
     * @param address Byte address of the conditional store.
     * @param value Value to store if the reservation is still valid.
     * @return `true` if the store succeeds, `false` otherwise.
     *
     * @note The precise policy for invalidating reservations belongs to the
     * implementation, but the return value should reflect the architectural
     * success or failure observed by the instruction semantics.
     */
    virtual bool store_conditional32(uint64_t address, uint32_t value) = 0;

    /**
     * @brief Perform a 64-bit store-conditional operation.
     *
     * @param address Byte address of the conditional store.
     * @param value Value to store if the reservation is still valid.
     * @return `true` if the store succeeds, `false` otherwise.
     *
     * @note The precise policy for invalidating reservations belongs to the
     * implementation, but the return value should reflect the architectural
     * success or failure observed by the instruction semantics.
     */
    virtual bool store_conditional64(uint64_t address, uint64_t value) = 0;

    /**
     * @brief Signal a synchronous architectural exception.
     *
     * @param cause Architectural exception cause code.
     * @param tval Additional exception-specific value, typically written to
     * `xtval`.
     *
     * @note This method represents architecturally visible trap signaling. The
     * implementation may update trap CSRs, privilege state, and the PC, or it
     * may record the event for a surrounding execution environment.
     */
    virtual void raise_exception(uint64_t cause, uint64_t tval) = 0;

    /**
     * @brief Signal an asynchronous architectural interrupt.
     *
     * @param cause Architectural interrupt cause code.
     *
     * @note This method does not prescribe when the interrupt is taken. It only
     * exposes the architectural event to the implementation.
     */
    virtual void raise_interrupt(uint64_t cause) = 0;

    /**
     * @brief Report the architectural integer register width.
     *
     * @return The hart XLEN in bits.
     *
     * @note The current project targets RV64 first, so most initial
     * implementations are expected to return `64`.
     */
    virtual std::size_t xlen() const = 0;
};

#endif
