#include "context.h"
#include "isr.h"
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

int a = 0;

void test() {
  a += 1;
  
  print_dword((uint32_t) a);
  print_char('\n');
  
  mk_thread_kill();
}

void debugging() {
  // print_qword((uint64_t) &mk_thread_ctx_restore);
  // print_qword((uint64_t) &test);
  print_qword((uint64_t) &mk_asm_timer_int);
}

void kernel_main() {
  mk_sys_init();
  mk_initial_checks();

  print_menu();
  
  mk_thread_create(&test);
  mk_thread_create(&test);
  
  // debugging();
  
  mk_start_timer();

  while(1);
}
