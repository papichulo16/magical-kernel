#include "includes/print.h"
#include "includes/int.h"

void kernel_main() {
  idt_init();

  print_clear();
  print_str("Welcome to Magical Kernel Ultra!\n");
  print_str("This is a work in progress, I am pretty busy and lazy so this will be shit and slow paced... but hopefully I get something done!\n");
}
