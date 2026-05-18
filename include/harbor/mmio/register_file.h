#ifndef HARBOR_MMIO_REGISTER_FILE_H
#define HARBOR_MMIO_REGISTER_FILE_H

#include "harbor/mmio/register_file_map.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

namespace harbor::mmio {

/**
 * @brief Fixed-size 32-bit memory-mapped register file.
 *
 * RegisterFile models the Harbor minimal test peripheral register storage. It
 * contains 16 registers, each 32 bits wide, for a 64-byte MMIO window.
 *
 * @note This class does not assign the MMIO base address and does not perform
 * bus decoding. A QEMU/SystemC adapter is expected to map the 64-byte window
 * into the guest physical address space and forward aligned 32-bit accesses to
 * this model.
 */
class RegisterFile {
public:
    static constexpr std::uint64_t DefaultBaseAddress = HARBOR_MMIO_REGISTER_FILE_BASE;
    static constexpr std::size_t RegisterCount = HARBOR_MMIO_REGISTER_FILE_REGISTER_COUNT;
    static constexpr std::size_t RegisterWidthBytes =
        HARBOR_MMIO_REGISTER_FILE_REGISTER_WIDTH_BYTES;
    static constexpr std::size_t AddressSpanBytes = HARBOR_MMIO_REGISTER_FILE_ADDRESS_SPAN_BYTES;

    /**
     * @brief Construct a register file with all registers reset to zero.
     */
    RegisterFile();

    /**
     * @brief Reset all registers to zero.
     */
    void reset();

    /**
     * @brief Read a 32-bit register by byte offset.
     *
     * @param offset Byte offset from the start of the register-file MMIO window.
     * @return The register value, or std::nullopt if the offset is outside the
     * register window or not aligned to 32 bits.
     */
    [[nodiscard]] std::optional<std::uint32_t> read32(std::uint64_t offset) const;

    /**
     * @brief Write a 32-bit register by byte offset.
     *
     * @param offset Byte offset from the start of the register-file MMIO window.
     * @param value Value to store in the selected register.
     * @return true if the write was accepted, false if the offset is outside the
     * register window or not aligned to 32 bits.
     */
    [[nodiscard]] bool write32(std::uint64_t offset, std::uint32_t value);

    /**
     * @brief Return whether an offset selects a valid 32-bit register.
     *
     * @param offset Byte offset from the start of the register-file MMIO window.
     * @return true for aligned offsets in the range [0, 64), false otherwise.
     */
    [[nodiscard]] static bool is_valid_register_offset(std::uint64_t offset);

private:
    std::array<std::uint32_t, RegisterCount> registers_{};
};

} // namespace harbor::mmio

#endif // HARBOR_MMIO_REGISTER_FILE_H
