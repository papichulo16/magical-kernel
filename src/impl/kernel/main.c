#include "print.h"
#include "inlines.c"

#include "idt.h"
#include "pic.h"

void mk_initial_checks() {
  print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_RED);
  
  if (!are_interrupts_enabled())
    print_str("[!] ERR: Interrupts are not enabled\n");
  
  if ((inb(0x21) & 1) == 1)
    print_str("[!] ERR: Timer IRQ is masked\n");

  if ((inb(0x21) & 2) == 1)
    print_str("[!] ERR: Keyboard IRQ is masked\n");

  print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_BLACK);
}

void kernel_main() {
  mk_pic_init();
  mk_idt_init();
  
  print_clear();

  mk_initial_checks();

  print_menu();
  
  while(1);
}
