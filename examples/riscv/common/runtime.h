#ifndef HARBOR_RISCV_RUNTIME_H
#define HARBOR_RISCV_RUNTIME_H

void runtime_putc(char value);
void runtime_puts(const char *value);
void runtime_exit(int code);

#endif
