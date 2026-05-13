.equ UART0_BASE, 0x10000000
.equ TEST_FINISHER_BASE, 0x100000
.equ TEST_FINISHER_PASS, 0x5555

.section .text.start
.global _start

_start:
  la t0, message
  li t1, UART0_BASE

print_loop:
  lbu t2, 0(t0)
  beqz t2, exit_success
  sb t2, 0(t1)
  addi t0, t0, 1
  j print_loop

exit_success:
  li t0, TEST_FINISHER_BASE
  li t1, TEST_FINISHER_PASS
  sw t1, 0(t0)

halt:
  wfi
  j halt

.section .rodata
message:
  .asciz "Hello from Harbor RISC-V bare metal on QEMU\n"

