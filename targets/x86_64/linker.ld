KERNEL_VMA = 0xffffffff80000000;
KERNEL_PMA = 0x1000000;

ENTRY(start)

SECTIONS
{
  /* -------------------------------
   Boot section — low memory, identity-mapped
   ------------------------------- */

  . = KERNEL_PMA;
  
  .boot : AT(ADDR(.boot))
  {
    KEEP(*(.multiboot_header))
    *(.boot.text)
    *(.boot.data)
    *(.boot.rodata)
  }

  .boot.bss : AT(ADDR(.boot.bss))
  {
    *(.boot.bss)
  }

  /* Align to next page after boot section */
  . = ALIGN(4096);
  _boot_end = .;

  /* -------------------------------
     Kernel proper — high virtual memory
     ------------------------------- */
  . = KERNEL_VMA + _boot_end;
  KERNEL_VIRT_START = .;

  .text : AT(_boot_end)
  {
    *(.text*)
  }

  KERNEL_VIRT_END = .;
}