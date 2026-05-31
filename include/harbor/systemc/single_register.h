#ifndef HARBOR_SYSTEMC_SINGLE_REGISTER_H
#define HARBOR_SYSTEMC_SINGLE_REGISTER_H

#include "harbor/mmio/transaction.h"

#include <cstdint>
#include <systemc>

namespace harbor::systemc {

/**
 * @brief Minimal SystemC-backed MMIO peripheral with one 32-bit register.
 *
 * SingleRegister is intentionally small. It proves that Harbor-owned SystemC
 * models can implement the environment-neutral MMIO target interface without
 * depending on QEMU types or QEMU source files.
 *
 * The model accepts aligned 32-bit accesses at local address 0. Other
 * addresses or access sizes return an error response.
 */
class SingleRegister final : public sc_core::sc_module, public harbor::mmio::Target {
public:
    /**
     * @brief Construct a single-register SystemC peripheral.
     *
     * @param name SystemC module name.
     * @param reset_value Value restored by reset().
     */
    explicit SingleRegister(sc_core::sc_module_name name, std::uint32_t reset_value = 0);

    /**
     * @brief Reset the register to its configured reset value.
     */
    void reset() override;

    /**
     * @brief Execute one environment-neutral MMIO transaction.
     *
     * @param transaction MMIO request using target-local addressing.
     * @return Transaction response.
     */
    [[nodiscard]] harbor::mmio::Response transport(
        const harbor::mmio::Transaction &transaction) override;

    /**
     * @brief Return the current register value.
     *
     * @return Current 32-bit register contents.
     */
    [[nodiscard]] std::uint32_t value() const;

private:
    std::uint32_t reset_value_ = 0;
    std::uint32_t value_ = 0;
};

} // namespace harbor::systemc

#endif // HARBOR_SYSTEMC_SINGLE_REGISTER_H
