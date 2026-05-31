#ifndef HARBOR_MMIO_TRANSACTION_H
#define HARBOR_MMIO_TRANSACTION_H

#include <cstdint>

namespace harbor::mmio {

/**
 * @brief Target-local byte address within an MMIO device window.
 *
 * QEMU or another host adapter is responsible for decoding guest physical
 * addresses into target-local addresses before calling Harbor-owned models.
 */
using LocalAddress = std::uint64_t;

/**
 * @brief Estimated transaction latency in picoseconds.
 *
 * A latency value of zero means the model does not currently annotate timing.
 */
using LatencyPs = std::uint64_t;

/**
 * @brief MMIO access direction.
 */
enum class AccessKind {
    Read,
    Write,
};

/**
 * @brief Supported MMIO access sizes.
 */
enum class AccessSize : std::uint8_t {
    Byte = 1,
    HalfWord = 2,
    Word = 4,
    DoubleWord = 8,
};

/**
 * @brief Result status for an MMIO transaction.
 */
enum class ResponseStatus {
    Ok,
    DecodeError,
    UnsupportedAccess,
};

/**
 * @brief Harbor-owned MMIO transaction request.
 *
 * This structure intentionally contains no QEMU, RISC-V, or SystemC/TLM types.
 * Adapters translate their environment-specific representation into this
 * request before calling Harbor models.
 */
struct Transaction {
    AccessKind kind = AccessKind::Read;
    LocalAddress address = 0;
    AccessSize size = AccessSize::Word;
    std::uint64_t write_value = 0;
};

/**
 * @brief Harbor-owned MMIO transaction response.
 */
struct Response {
    ResponseStatus status = ResponseStatus::Ok;
    std::uint64_t read_value = 0;
    LatencyPs latency = 0;

    /**
     * @brief Return whether the transaction completed successfully.
     *
     * @return true if status is ResponseStatus::Ok, false otherwise.
     */
    [[nodiscard]] bool ok() const;
};

/**
 * @brief Abstract MMIO endpoint implemented by Harbor models.
 *
 * QEMU-facing adapters, future SystemC bridges, and tests should depend on
 * this interface rather than concrete model classes when a generic MMIO target
 * is sufficient.
 */
class Target {
public:
    virtual ~Target() = default;

    /**
     * @brief Reset the target to its initial state.
     */
    virtual void reset() = 0;

    /**
     * @brief Execute a single MMIO transaction.
     *
     * @param transaction Environment-neutral MMIO request.
     * @return Environment-neutral transaction response.
     */
    [[nodiscard]] virtual Response transport(const Transaction &transaction) = 0;
};

} // namespace harbor::mmio

#endif // HARBOR_MMIO_TRANSACTION_H
