#include "page_temp.h"
#include "print.h"
#include "inlines.c"

#include "idt.h"
#include "pic.h"

void mk_initial_checks() {
  if (!are_interrupts_enabled())
    print_error("[!] ERR: Interrupts are not enabled\n");
  
  if ((inb(0x21) & 1) == 1)
    print_error("[!] ERR: Timer IRQ is masked\n");

  if ((inb(0x21) & 2) == 1)
    print_error("[!] ERR: Keyboard IRQ is masked\n");
}

void mk_sys_init() {
  mk_pic_init();
  mk_idt_init();
  mk_temp_page_init();

  print_clear();
}

void kernel_main() {
  mk_sys_init();
  mk_initial_checks();

  print_menu();

  while(1);
}
