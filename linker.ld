SECTIONS
{
  . = 0x80000000;
  .text :
  {
    boot.o (.text)
    *(.text)
  }
  .data : { *(.data) }
  .bss  : { *(.bss) }

  /DISCARD/ : {
        *(.riscv.attributes)
  }
}
