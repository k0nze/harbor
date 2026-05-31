#include "harbor/qemu/mmio_adapter.h"

#include <cassert>
#include <cstdint>
#include <systemc>

namespace {

void existing_c_abi_accesses_systemc_backed_register() {
    HarborQemuMmioDevice *device = harbor_qemu_systemc_single_register_create(0x12345678U);
    assert(device != nullptr);

    std::uint32_t value = 0;
    assert(harbor_qemu_mmio_device_read32(device, 0, &value));
    assert(value == 0x12345678U);

    assert(harbor_qemu_mmio_device_write32(device, 0, 0xa5a55a5aU));
    assert(harbor_qemu_mmio_device_read32(device, 0, &value));
    assert(value == 0xa5a55a5aU);

    harbor_qemu_mmio_device_reset(device);
    assert(harbor_qemu_mmio_device_read32(device, 0, &value));
    assert(value == 0x12345678U);

    assert(!harbor_qemu_mmio_device_read32(device, 4, &value));
    assert(!harbor_qemu_mmio_device_write32(device, 4, 0xffffffffU));

    harbor_qemu_mmio_device_destroy(device);
}

} // namespace

int sc_main(int, char *[]) {
    existing_c_abi_accesses_systemc_backed_register();
    return 0;
}
