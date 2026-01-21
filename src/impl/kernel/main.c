#include "cmd.h"
#include "print.h"
#include "inlines.c"

#include "page.h"
#include "slab.h"
#include "virt.h"
#include "idt.h"
#include "isr.h"
#include "pic.h"
#include "pci.h"
#include "thread.h"
#include "drivers/ps2.h"
#include "drivers/ahci.h"
#include "drivers/ext2.h"

#include <stdint.h>

void initial_checks() {
  if (!are_interrupts_enabled())
    print_error("[!] ERR: Interrupts are not enabled\n");
  
  if ((inb(0x21) & 2) == 1)
    print_error("[!] ERR: Keyboard IRQ is masked\n");
}

void sys_init() {
  mk_page_alloc_init();

  mk_pic_init(); // make sure pic is initialized before setting up idt
  mk_idt_init();

  mk_virt_init();

  mk_ahci_init();
  mk_ext2_init();
}

void start_keyboard() {
    mk_irq_clear_mask(1); // enable keyboard interrupts

  if ((inb(0x21) & 2) == 1)
    print_error("[!] ERR: PS/2 Keyboard IRQ is masked\n");
}

void start_timer() {
  mk_irq_clear_mask(0); // enable timer interrupts
  
  if ((inb(0x21) & 1) == 1)
    print_error("[!] ERR: Timer IRQ is masked\n");
}

void reg_cmds() {
  mk_register_cmd("help", "list all cmds", &cmd_help, 1);
  mk_register_cmd("clear", "clear screen", &print_user_clear, 1);
}

void idle_thread() {
  while(1);
}

void initialize_tasks() {
  mk_thread_create(&idle_thread, "idle_task");
  mk_thread_create(&mk_ps2_keyboard_driver, "ps2_keyboard_task");
}

void kernel_main() {

  print_clear();
  print_menu();

  sys_init();
  initial_checks();

  reg_cmds();
  initialize_tasks();

  start_keyboard();
  start_timer();

  while(1);
}
