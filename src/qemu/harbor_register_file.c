/*
 * QEMU-facing Harbor SystemC MMIO device.
 *
 * This file is a thin QEMU C shim. Device behavior lives behind the Harbor
 * QEMU MMIO adapter so Harbor SystemC/TLM models remain the source of truth.
 */

#include "qemu/osdep.h"
#include "hw/core/resettable.h"
#include "hw/core/sysbus.h"
#include "qemu/error-report.h"
#include "qemu/log.h"
#include "qemu/module.h"

#include "harbor/qemu/mmio_adapter.h"

#define TYPE_HARBOR_REGISTER_FILE "harbor-register-file"

#define HARBOR_QEMU_REGISTER_WIDTH_BYTES 4U
#define HARBOR_QEMU_MMIO_SPAN_BYTES HARBOR_QEMU_REGISTER_WIDTH_BYTES
#define HARBOR_QEMU_SYSTEMC_SINGLE_REGISTER_RESET 0x12345678U

OBJECT_DECLARE_SIMPLE_TYPE(HarborRegisterFileState, HARBOR_REGISTER_FILE)

struct HarborRegisterFileState {
    SysBusDevice parent_obj;
    MemoryRegion mmio;
    HarborQemuMmioDevice *device;
};

static bool harbor_register_file_valid_access(hwaddr addr, unsigned int size)
{
    return size == HARBOR_QEMU_REGISTER_WIDTH_BYTES &&
           (addr % HARBOR_QEMU_REGISTER_WIDTH_BYTES) == 0 &&
           addr < HARBOR_QEMU_MMIO_SPAN_BYTES;
}

static uint64_t harbor_register_file_read(void *opaque, hwaddr addr,
                                          unsigned int size)
{
    HarborRegisterFileState *s = HARBOR_REGISTER_FILE(opaque);
    uint32_t value = 0;

    if (!harbor_register_file_valid_access(addr, size) ||
        !harbor_qemu_mmio_device_read32(s->device, addr, &value)) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: invalid read addr=0x%" HWADDR_PRIx " size=%u\n",
                      __func__, addr, size);
        return 0;
    }

    return value;
}

static void harbor_register_file_write(void *opaque, hwaddr addr,
                                       uint64_t value, unsigned int size)
{
    HarborRegisterFileState *s = HARBOR_REGISTER_FILE(opaque);

    if (!harbor_register_file_valid_access(addr, size) ||
        !harbor_qemu_mmio_device_write32(s->device, addr, (uint32_t)value)) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: invalid write addr=0x%" HWADDR_PRIx
                      " size=%u value=0x%" PRIx64 "\n",
                      __func__, addr, size, value);
    }
}

static const MemoryRegionOps harbor_register_file_ops = {
    .read = harbor_register_file_read,
    .write = harbor_register_file_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = {
        .min_access_size = HARBOR_QEMU_REGISTER_WIDTH_BYTES,
        .max_access_size = HARBOR_QEMU_REGISTER_WIDTH_BYTES,
        .unaligned = false,
    },
    .impl = {
        .min_access_size = HARBOR_QEMU_REGISTER_WIDTH_BYTES,
        .max_access_size = HARBOR_QEMU_REGISTER_WIDTH_BYTES,
        .unaligned = false,
    },
};

static void harbor_register_file_reset_enter(Object *obj, ResetType type)
{
    HarborRegisterFileState *s = HARBOR_REGISTER_FILE(obj);

    harbor_qemu_mmio_device_reset(s->device);
}

static void harbor_register_file_instance_finalize(Object *obj)
{
    HarborRegisterFileState *s = HARBOR_REGISTER_FILE(obj);

    harbor_qemu_mmio_device_destroy(s->device);
    s->device = NULL;
}

static void harbor_register_file_init(Object *obj)
{
    HarborRegisterFileState *s = HARBOR_REGISTER_FILE(obj);

    s->device = harbor_qemu_systemc_single_register_create(
        HARBOR_QEMU_SYSTEMC_SINGLE_REGISTER_RESET);
    if (s->device == NULL) {
        error_report("%s: failed to allocate Harbor SystemC MMIO adapter",
                     TYPE_HARBOR_REGISTER_FILE);
        abort();
    }

    memory_region_init_io(&s->mmio, obj, &harbor_register_file_ops, s,
                          TYPE_HARBOR_REGISTER_FILE,
                          HARBOR_QEMU_MMIO_SPAN_BYTES);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);
}

static void harbor_register_file_class_init(ObjectClass *klass, const void *data)
{
    ResettableClass *rc = RESETTABLE_CLASS(klass);

    rc->phases.enter = harbor_register_file_reset_enter;
}

static const TypeInfo harbor_register_file_info = {
    .name = TYPE_HARBOR_REGISTER_FILE,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(HarborRegisterFileState),
    .instance_init = harbor_register_file_init,
    .instance_finalize = harbor_register_file_instance_finalize,
    .class_init = harbor_register_file_class_init,
};

static void harbor_register_file_register_types(void)
{
    type_register_static(&harbor_register_file_info);
}

type_init(harbor_register_file_register_types)
