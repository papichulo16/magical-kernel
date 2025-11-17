#include "cmd.h"
#include "print.h"
#include "inlines.c"

#include "page.h"
#include "virt.h"
#include "idt.h"
#include "isr.h"
#include "pic.h"
#include "thread.h"

#include <stdint.h>

void* sema;

void initial_checks() {
  if (!are_interrupts_enabled())
    print_error("[!] ERR: Interrupts are not enabled\n");
  
  if ((inb(0x21) & 2) == 1)
    print_error("[!] ERR: Keyboard IRQ is masked\n");
}

void sys_init() {
  mk_page_alloc_init();
  mk_virt_init();
  mk_idt_init();
  mk_pic_init();
}

void start_timer() {
  mk_irq_clear_mask(0); // enable timer interrupts
  
  if ((inb(0x21) & 1) == 1)
    print_error("[!] ERR: Timer IRQ is masked\n");
}

void test(char* s) {
  print_str(s);
  print_char('\n');
}

void reg_cmds() {
  mk_register_cmd("help", "list all cmds", &cmd_help, 1);
  mk_register_cmd("clear", "clear screen", &print_user_clear, 1);
}

void alice() {
  mk_sema_take(sema); 

  print_str("alice\n");
  
  mk_thread_kill();
}

void bob() {
  print_str("bob\n");
  
  mk_sema_give(sema);

  mk_thread_kill();
}

void idle_thread() {
  while(1);
}

void debugging() {
  uint32_t* addr = (uint32_t *) 0x1fffff0 + 0x010000000000;
  *(uint32_t *) addr = (uint32_t) 0x41414141;
  uint32_t test = *(uint32_t *) addr;
  
  print_dword(test);
}

void kernel_main() {
  sys_init();
  initial_checks();
  
  reg_cmds();

  print_clear();
  print_menu();

  mk_create_sema(&sema, -1);
  
  mk_thread_create(&idle_thread, 15);
  // mk_thread_create(&mk_keyboard, 0);
  mk_thread_create(&alice, 14);
  mk_thread_create(&bob, 15);
  // debugging();
  
  
  start_timer();

  while(1);
}
