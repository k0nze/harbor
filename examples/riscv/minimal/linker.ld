OUTPUT_ARCH("riscv")
ENTRY(_start)

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
    *(.bss)
    *(.bss.*)
    *(COMMON)
  } > ram
}

