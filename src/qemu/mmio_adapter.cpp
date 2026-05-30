#include "harbor/qemu/mmio_adapter.h"

#include "harbor/mmio/register_file.h"

#include <memory>
#include <new>

struct HarborQemuMmioDevice {
    harbor::mmio::RegisterFile register_file;
};

HarborQemuMmioDevice *harbor_qemu_register_file_create(void) {
    try {
        return new HarborQemuMmioDevice{};
    } catch (const std::bad_alloc &) {
        return nullptr;
    }
}

void harbor_qemu_mmio_device_destroy(HarborQemuMmioDevice *device) {
    delete device;
}

void harbor_qemu_mmio_device_reset(HarborQemuMmioDevice *device) {
    if (device == nullptr) {
        return;
    }

    device->register_file.reset();
}

bool harbor_qemu_mmio_device_read32(HarborQemuMmioDevice *device,
                                    uint64_t offset,
                                    uint32_t *value) {
    if (device == nullptr || value == nullptr) {
        return false;
    }

    const auto read_value = device->register_file.read32(offset);
    if (!read_value.has_value()) {
        return false;
    }

    *value = *read_value;
    return true;
}

bool harbor_qemu_mmio_device_write32(HarborQemuMmioDevice *device,
                                     uint64_t offset,
                                     uint32_t value) {
    if (device == nullptr) {
        return false;
    }

    return device->register_file.write32(offset, value);
}
