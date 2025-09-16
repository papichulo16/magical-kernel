#include "includes/print.h"
#include "includes/inlines.c"

#include "interrupts/idt.h"
#include "interrupts/pic.h"

void initial_checks() {
  print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_RED);
  
  if (!are_interrupts_enabled())
    print_str("[!] ERR: Interrupts are not enabled\n");
  
  if ((inb(0x21) & 2) == 1)
    print_str("[!] ERR: Keyboard not initialized\n");

  print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_BLACK);
}

void kernel_main() {
  pic_init();
  idt_init();
  
  print_clear();

  initial_checks();

  print_menu();
  
  while(1);
}
