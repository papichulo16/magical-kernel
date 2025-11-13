#include "print.h"
#include "inlines.c"

#include "page.h"
#include "virt.h"
#include "idt.h"
#include "pic.h"
#include "thread.h"

#include <stdint.h>

void mk_initial_checks() {
  if (!are_interrupts_enabled())
    print_error("[!] ERR: Interrupts are not enabled\n");
  
  if ((inb(0x21) & 2) == 1)
    print_error("[!] ERR: Keyboard IRQ is masked\n");
}

void mk_sys_init() {
  mk_pic_init();
  mk_idt_init();
  mk_page_alloc_init();
  mk_virt_init();

  print_clear();
}

void mk_start_timer() {
  mk_irq_clear_mask(0); // enable timer interrupts
  
  if ((inb(0x21) & 1) == 1)
    print_error("[!] ERR: Timer IRQ is masked\n");
}

void idle_thread() {
  while(1);
}

void alice() {
  print_str("alice\n");

  mk_thread_kill();
}

void debugging() {
  uint32_t* addr = (uint32_t *) 0x1fffff0 + 0x010000000000;
  *(uint32_t *) addr = (uint32_t) 0x41414141;
  uint32_t test = *(uint32_t *) addr;
  
  print_dword(test);
}

void kernel_main() {
  mk_sys_init();
  mk_initial_checks();

  print_menu();
  
  // mk_thread_create(&idle_thread);
  // mk_thread_create(&alice);
  // debugging();
  
  
  // mk_start_timer();

  while(1);
}
