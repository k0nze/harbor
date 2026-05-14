OUTPUT_ARCH("riscv")
ENTRY(_start)

__STACK_SIZE = 0x4000;

MEMORY
{
  ram (wxa) : ORIGIN = 0x80000000, LENGTH = 128M
}

SECTIONS
{
  . = ORIGIN(ram);

  .text : {
    *(.text.start)
    *(.text)
    *(.text.*)
  } > ram

  .rodata : {
    *(.rodata)
    *(.rodata.*)
  } > ram

  .data : {
    *(.data)
    *(.data.*)
  } > ram

  .bss (NOLOAD) : {
    __bss_start = .;
    *(.bss)
    *(.bss.*)
    *(COMMON)
    . = ALIGN(8);
    __bss_end = .;
  } > ram

  .stack ALIGN(16) (NOLOAD) : {
    _stack_bottom = .;
    . += __STACK_SIZE;
    . = ALIGN(16);
    _stack_top = .;
  } > ram

  __global_pointer$ = _stack_top;
}
