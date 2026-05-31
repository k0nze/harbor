#ifndef HARBOR_QEMU_MMIO_DEVICE_H
#define HARBOR_QEMU_MMIO_DEVICE_H

#include "harbor/mmio/transaction.h"
#include "harbor/qemu/mmio_adapter.h"

#include <memory>

struct HarborQemuMmioDevice {
    explicit HarborQemuMmioDevice(std::unique_ptr<harbor::mmio::Target> target)
        : target{std::move(target)} {}

    std::unique_ptr<harbor::mmio::Target> target;
};

#endif // HARBOR_QEMU_MMIO_DEVICE_H
