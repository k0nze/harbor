#include "runtime.h"

#include <stdint.h>

#define HARBOR_SYSTEMC_MMIO_BASE UINT64_C(0x10010000)
#define HARBOR_SYSTEMC_MMIO_RESET_VALUE UINT32_C(0x12345678)
#define HARBOR_SYSTEMC_MMIO_WRITE_VALUE UINT32_C(0xa5a55a5a)

static volatile uint32_t *const systemc_register =
    (volatile uint32_t *)HARBOR_SYSTEMC_MMIO_BASE;

int main(void) {
    runtime_puts("Checking Harbor SystemC MMIO register\n");

    if (*systemc_register != HARBOR_SYSTEMC_MMIO_RESET_VALUE) {
        runtime_puts("SystemC MMIO reset value mismatch\n");
        return 1;
    }

    *systemc_register = HARBOR_SYSTEMC_MMIO_WRITE_VALUE;
    if (*systemc_register != HARBOR_SYSTEMC_MMIO_WRITE_VALUE) {
        runtime_puts("SystemC MMIO write/read mismatch\n");
        return 2;
    }

    runtime_puts("SystemC MMIO check passed\n");
    return 0;
}
