#ifndef HARBOR_QEMU_MMIO_ADAPTER_H
#define HARBOR_QEMU_MMIO_ADAPTER_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque Harbor MMIO device handle used by QEMU C device code.
 *
 * QEMU must treat this object as an opaque adapter endpoint. The implementation
 * lives in Harbor C++ code and forwards transactions into SystemC/TLM models.
 */
typedef struct HarborQemuMmioDevice HarborQemuMmioDevice;

/**
 * @brief Create a SystemC-backed single-register MMIO device.
 *
 * The returned handle is accessed through the generic Harbor QEMU MMIO C ABI.
 *
 * @param reset_value Initial value restored by reset.
 * @return New device handle, or NULL if allocation fails.
 */
HarborQemuMmioDevice *harbor_qemu_systemc_single_register_create(uint32_t reset_value);

/**
 * @brief Destroy a Harbor MMIO device handle.
 *
 * @param device Device handle returned by a Harbor QEMU adapter create
 * function. Passing NULL is allowed.
 */
void harbor_qemu_mmio_device_destroy(HarborQemuMmioDevice *device);

/**
 * @brief Reset a Harbor MMIO device to its initial architectural state.
 *
 * @param device Device handle.
 */
void harbor_qemu_mmio_device_reset(HarborQemuMmioDevice *device);

/**
 * @brief Read a 32-bit MMIO register through the Harbor adapter.
 *
 * @param device Device handle.
 * @param offset Byte offset from the start of the MMIO window.
 * @param value Output storage for the read value.
 * @return true if the read was accepted, false for invalid offset or NULL
 * arguments.
 */
bool harbor_qemu_mmio_device_read32(HarborQemuMmioDevice *device,
                                    uint64_t offset,
                                    uint32_t *value);

/**
 * @brief Write a 32-bit MMIO register through the Harbor adapter.
 *
 * @param device Device handle.
 * @param offset Byte offset from the start of the MMIO window.
 * @param value Value to write.
 * @return true if the write was accepted, false for invalid offset or NULL
 * device.
 */
bool harbor_qemu_mmio_device_write32(HarborQemuMmioDevice *device,
                                     uint64_t offset,
                                     uint32_t value);

#ifdef __cplusplus
}
#endif

#endif // HARBOR_QEMU_MMIO_ADAPTER_H
