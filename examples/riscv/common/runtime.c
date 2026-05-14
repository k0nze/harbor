#include "runtime.h"

#define HARBOR_UART0_BASE 0x10000000UL
#define HARBOR_TEST_FINISHER_BASE 0x100000UL
#define HARBOR_TEST_FINISHER_PASS 0x5555U
#define HARBOR_TEST_FINISHER_FAIL 0x3333U

static volatile unsigned char *const uart0 = (volatile unsigned char *)HARBOR_UART0_BASE;
static volatile unsigned int *const test_finisher = (volatile unsigned int *)HARBOR_TEST_FINISHER_BASE;

void runtime_putc(char value)
{
  *uart0 = (unsigned char)value;
}

void runtime_puts(const char *value)
{
  while (*value != '\0') {
    runtime_putc(*value);
    ++value;
  }
}

void runtime_exit(int code)
{
  if (code == 0) {
    *test_finisher = HARBOR_TEST_FINISHER_PASS;
  } else {
    *test_finisher = HARBOR_TEST_FINISHER_FAIL | ((unsigned int)code << 16);
  }

  for (;;) {
    __asm__ volatile("wfi");
  }
}
