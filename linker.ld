OUTPUT_ARCH("riscv")

MEMORY
{
	ram(wxa!ri): ORIGIN = 0x80000000, LENGTH = 0x4000
}

SECTIONS
{
  . = 0x80000000;
  .text :
  {
    boot.o (.text)
    *(.text)
  } > ram
  .data : { *(.data) } > ram
  .bss  : { *(.bss) } > ram

  /DISCARD/ : {
        *(.riscv.attributes)
  }
}
