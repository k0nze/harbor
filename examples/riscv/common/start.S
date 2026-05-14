.section .text.start
.global _start

_start:
  la gp, __global_pointer$
  la sp, _stack_top

  la t0, __bss_start
  la t1, __bss_end

clear_bss:
  bgeu t0, t1, call_main
  sd zero, 0(t0)
  addi t0, t0, 8
  j clear_bss

call_main:
  call main
  tail runtime_exit
