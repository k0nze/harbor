#include "harbor/qemu/mmio_adapter.h"

#include "harbor/systemc/single_register.h"
#include "mmio_device.h"

#include <memory>
#include <new>

namespace {

class SystemcSingleRegisterTarget final : public harbor::mmio::Target {
public:
    explicit SystemcSingleRegisterTarget(uint32_t reset_value)
        : register_{"harbor_qemu_systemc_single_register", reset_value} {}

    void reset() override {
        register_.reset();
    }

    [[nodiscard]] harbor::mmio::Response transport(
        const harbor::mmio::Transaction &transaction) override {
        return register_.transport(transaction);
    }

private:
    harbor::systemc::SingleRegister register_;
};

} // namespace

HarborQemuMmioDevice *harbor_qemu_systemc_single_register_create(uint32_t reset_value) {
    try {
        return new HarborQemuMmioDevice{
            std::make_unique<SystemcSingleRegisterTarget>(reset_value)};
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

    device->target->reset();
}

bool harbor_qemu_mmio_device_read32(HarborQemuMmioDevice *device,
                                    uint64_t offset,
                                    uint32_t *value) {
    if (device == nullptr || value == nullptr) {
        return false;
    }

    const auto response = device->target->transport({
        .kind = harbor::mmio::AccessKind::Read,
        .address = offset,
        .size = harbor::mmio::AccessSize::Word,
    });
    if (!response.ok()) {
        return false;
    }

    *value = static_cast<uint32_t>(response.read_value);
    return true;
}

bool harbor_qemu_mmio_device_write32(HarborQemuMmioDevice *device,
                                     uint64_t offset,
                                     uint32_t value) {
    if (device == nullptr) {
        return false;
    }

    const auto response = device->target->transport({
        .kind = harbor::mmio::AccessKind::Write,
        .address = offset,
        .size = harbor::mmio::AccessSize::Word,
        .write_value = value,
    });

    return response.ok();
}
