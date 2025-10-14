#include "context.h"
#include "page_temp.h"
#include "print.h"
#include "inlines.c"

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
  mk_temp_page_init();

  print_clear();
}

void mk_start_timer() {
  mk_irq_clear_mask(0); // enable timer interrupts
  
  if ((inb(0x21) & 1) == 1)
    print_error("[!] ERR: Timer IRQ is masked\n");
}

void test() {
  print_str("Hello from a thread\n");
  
  mk_thread_kill();
}

void test2() {
  print_str("Hello from a final thread\n");
  
  mk_thread_kill();
}

void idle() {
  while(1);
}

void debugging() {
  print_qword((uint64_t) &mk_thread_ctx_restore);
  print_qword((uint64_t) &test);
}

void kernel_main() {
  mk_sys_init();
  mk_initial_checks();

  print_menu();
  
  mk_thread_create(&idle);
  mk_thread_create(&test);
  // mk_thread_create(&test2);
  
  // debugging();
  
  // mk_start_timer();

  while(1);
}
