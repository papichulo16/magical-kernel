ENTRY(start)

SECTIONS
{
  /* the operating system starts one megabyte in */
  . = 0x1000000;

  .boot :
  {
    KEEP(*(.multiboot_header))
  }

  .text :
  {
    *(.text)
  }
}
