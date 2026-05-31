#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define HARBOR_SYSTEMC_REGISTER_BASE UINT64_C(0x10010000)
#define HARBOR_SYSTEMC_REGISTER_WIDTH_BYTES 4U
#define HARBOR_SYSTEMC_REGISTER_RESET_VALUE UINT32_C(0x12345678)
#define HARBOR_SYSTEMC_REGISTER_WRITE_VALUE UINT32_C(0xa5a55a5a)

enum access_width {
    ACCESS_WIDTH_8 = 1,
    ACCESS_WIDTH_16 = 2,
    ACCESS_WIDTH_32 = 4,
    ACCESS_WIDTH_64 = 8,
};

static void print_usage(const char *program)
{
    printf("Usage:\n");
    printf("  %s --help\n", program);
    printf("  %s --dry-run <address> [width]\n", program);
    printf("  %s read <address> [width]\n", program);
    printf("  %s write <address> <value> [width]\n", program);
    printf("  %s systemc-register --dry-run\n", program);
    printf("  %s systemc-register check\n", program);
    printf("\n");
    printf("Arguments:\n");
    printf("  address  Physical MMIO address, for example 0x10010000.\n");
    printf("  value    Integer value to write.\n");
    printf("  width    Access width in bits: 8, 16, 32, or 64. Defaults to 32.\n");
    printf("\n");
    printf("SystemC register:\n");
    printf("  base     0x%" PRIx64 "\n", HARBOR_SYSTEMC_REGISTER_BASE);
    printf("  width    32 bits\n");
    printf("  span     0x%x bytes\n", HARBOR_SYSTEMC_REGISTER_WIDTH_BYTES);
}

static int parse_u64(const char *text, uint64_t *value)
{
    char *end = NULL;

    errno = 0;
    *value = strtoull(text, &end, 0);
    if (errno != 0 || end == text || *end != '\0') {
        return -1;
    }

    return 0;
}

static int parse_width(const char *text, enum access_width *width)
{
    uint64_t bits = 32;

    if (text != NULL && parse_u64(text, &bits) != 0) {
        return -1;
    }

    switch (bits) {
    case 8:
        *width = ACCESS_WIDTH_8;
        return 0;
    case 16:
        *width = ACCESS_WIDTH_16;
        return 0;
    case 32:
        *width = ACCESS_WIDTH_32;
        return 0;
    case 64:
        *width = ACCESS_WIDTH_64;
        return 0;
    default:
        return -1;
    }
}

static uint64_t width_mask(enum access_width width)
{
    switch (width) {
    case ACCESS_WIDTH_8:
        return UINT64_C(0xff);
    case ACCESS_WIDTH_16:
        return UINT64_C(0xffff);
    case ACCESS_WIDTH_32:
        return UINT64_C(0xffffffff);
    case ACCESS_WIDTH_64:
        return UINT64_MAX;
    default:
        return 0;
    }
}

static uint64_t read_mmio(volatile void *address, enum access_width width)
{
    switch (width) {
    case ACCESS_WIDTH_8:
        return *(volatile uint8_t *)address;
    case ACCESS_WIDTH_16:
        return *(volatile uint16_t *)address;
    case ACCESS_WIDTH_32:
        return *(volatile uint32_t *)address;
    case ACCESS_WIDTH_64:
        return *(volatile uint64_t *)address;
    default:
        return 0;
    }
}

static void write_mmio(volatile void *address, enum access_width width, uint64_t value)
{
    switch (width) {
    case ACCESS_WIDTH_8:
        *(volatile uint8_t *)address = (uint8_t)value;
        break;
    case ACCESS_WIDTH_16:
        *(volatile uint16_t *)address = (uint16_t)value;
        break;
    case ACCESS_WIDTH_32:
        *(volatile uint32_t *)address = (uint32_t)value;
        break;
    case ACCESS_WIDTH_64:
        *(volatile uint64_t *)address = value;
        break;
    }
}

static int map_and_access(const char *operation, uint64_t physical_address,
                          enum access_width width, uint64_t write_value,
                          uint64_t readback_expected,
                          int validate_read)
{
    const long page_size = sysconf(_SC_PAGESIZE);
    uint64_t page_mask = 0;
    uint64_t page_base = 0;
    uint64_t page_offset = 0;
    size_t map_size = 0;
    int fd = -1;
    void *mapping = MAP_FAILED;
    volatile void *mmio_address = NULL;
    uint64_t value = 0;

    if (page_size <= 0) {
        fprintf(stderr, "Could not determine system page size\n");
        return 1;
    }

    if ((physical_address % (uint64_t)width) != 0) {
        fprintf(stderr, "Address 0x%" PRIx64 " is not aligned to %u-bit access\n",
                physical_address, (unsigned int)width * 8U);
        return 1;
    }

    page_mask = (uint64_t)page_size - UINT64_C(1);
    page_base = physical_address & ~page_mask;
    page_offset = physical_address - page_base;
    map_size = (size_t)page_offset + (size_t)width;

    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        fprintf(stderr, "Could not open /dev/mem: %s\n", strerror(errno));
        return 1;
    }

    mapping = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                   (off_t)page_base);
    if (mapping == MAP_FAILED) {
        fprintf(stderr, "Could not map physical address 0x%" PRIx64 ": %s\n",
                physical_address, strerror(errno));
        close(fd);
        return 1;
    }

    mmio_address = (volatile uint8_t *)mapping + page_offset;

    if (strcmp(operation, "read") == 0) {
        value = read_mmio(mmio_address, width);
        printf("read 0x%" PRIx64 " width %u -> 0x%" PRIx64 "\n",
               physical_address, (unsigned int)width * 8U, value);

        if (validate_read && value != (readback_expected & width_mask(width))) {
            fprintf(stderr,
                    "Read mismatch at 0x%" PRIx64 ": expected 0x%" PRIx64
                    ", got 0x%" PRIx64 "\n",
                    physical_address, readback_expected & width_mask(width), value);
            munmap(mapping, map_size);
            close(fd);
            return 1;
        }
    } else {
        const uint64_t expected = write_value & width_mask(width);

        write_mmio(mmio_address, width, expected);
        printf("write 0x%" PRIx64 " width %u <- 0x%" PRIx64 "\n",
               physical_address, (unsigned int)width * 8U, expected);
    }

    munmap(mapping, map_size);
    close(fd);
    return 0;
}

static int systemc_register_dry_run(void)
{
    printf("systemc-register dry-run base 0x%" PRIx64 " width 32 span 0x%x\n",
           HARBOR_SYSTEMC_REGISTER_BASE,
           HARBOR_SYSTEMC_REGISTER_WIDTH_BYTES);
    printf("systemc-register reset value 0x%08" PRIx32 "\n",
           HARBOR_SYSTEMC_REGISTER_RESET_VALUE);
    return 0;
}

static int systemc_register_check(void)
{
    printf("systemc-register read reset\n");
    if (map_and_access("read", HARBOR_SYSTEMC_REGISTER_BASE, ACCESS_WIDTH_32, 0,
                       HARBOR_SYSTEMC_REGISTER_RESET_VALUE, 1) != 0) {
        return 1;
    }

    printf("systemc-register write pass\n");
    if (map_and_access("write", HARBOR_SYSTEMC_REGISTER_BASE, ACCESS_WIDTH_32,
                       HARBOR_SYSTEMC_REGISTER_WRITE_VALUE, 0, 0) != 0) {
        return 1;
    }

    printf("systemc-register read pass\n");
    if (map_and_access("read", HARBOR_SYSTEMC_REGISTER_BASE, ACCESS_WIDTH_32, 0,
                       HARBOR_SYSTEMC_REGISTER_WRITE_VALUE, 1) != 0) {
        return 1;
    }

    printf("systemc-register check passed\n");
    return 0;
}

int main(int argc, char **argv)
{
    const char *operation = NULL;
    uint64_t physical_address = 0;
    uint64_t write_value = 0;
    enum access_width width = ACCESS_WIDTH_32;

    if (argc < 2 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_usage(argv[0]);
        return argc < 2 ? 1 : 0;
    }

    operation = argv[1];

    if (strcmp(operation, "systemc-register") == 0) {
        if (argc != 3) {
            print_usage(argv[0]);
            return 1;
        }

        if (strcmp(argv[2], "--dry-run") == 0) {
            return systemc_register_dry_run();
        }

        if (strcmp(argv[2], "check") == 0) {
            return systemc_register_check();
        }

        print_usage(argv[0]);
        return 1;
    }

    if (strcmp(operation, "--dry-run") == 0) {
        if (argc < 3 || argc > 4 || parse_u64(argv[2], &physical_address) != 0 ||
            parse_width(argc == 4 ? argv[3] : NULL, &width) != 0) {
            print_usage(argv[0]);
            return 1;
        }

        printf("dry-run address 0x%" PRIx64 " width %u\n",
               physical_address, (unsigned int)width * 8U);
        return 0;
    }

    if (strcmp(operation, "read") == 0) {
        if (argc < 3 || argc > 4 || parse_u64(argv[2], &physical_address) != 0 ||
            parse_width(argc == 4 ? argv[3] : NULL, &width) != 0) {
            print_usage(argv[0]);
            return 1;
        }

        return map_and_access(operation, physical_address, width, 0, 0, 0);
    }

    if (strcmp(operation, "write") == 0) {
        if (argc < 4 || argc > 5 || parse_u64(argv[2], &physical_address) != 0 ||
            parse_u64(argv[3], &write_value) != 0 ||
            parse_width(argc == 5 ? argv[4] : NULL, &width) != 0) {
            print_usage(argv[0]);
            return 1;
        }

        return map_and_access(operation, physical_address, width, write_value, 0, 0);
    }

    print_usage(argv[0]);
    return 1;
}
