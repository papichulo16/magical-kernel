#include "cmd.h"
#include "print.h"
#include "inlines.c"

#include "page.h"
#include "virt.h"
#include "idt.h"
#include "pic.h"
#include "thread.h"

#include <stdint.h>

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
  
  mk_thread_create(&idle_thread);
  // debugging();
  
  
  start_timer();

  while(1);
}
