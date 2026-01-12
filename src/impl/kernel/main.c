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

char wbuf[512] __attribute((aligned(16)));
char rbuf[512] __attribute((aligned(16)));

void disk_rw_test() {
  int c = 512;
  //char* wbuf = mkmalloc(c);
  //char* rbuf = mkmalloc(c);

  for (int i = 0; i < c; i++) {
    wbuf[i] = 'A';
  }

  print_str("hello");

  struct ahci_rw_port_t* port = mk_g_ahci_head();

  cache_flush_all();

  if (!mk_ahci_write(port, 100, 1, wbuf)) {
    print_error("write fail\n");

    while(1);
  }
  
  if (!mk_ahci_read(port, 100, 1, rbuf)) {
    print_error("read fail\n");

    while(1);
  }

  cache_flush_all();

  print_str("buf: ");
  print_str(rbuf);

  mk_thread_kill();
}

void initialize_tasks() {
  mk_thread_create(&idle_thread, "idle_task");
  mk_thread_create(&mk_ps2_keyboard_driver, "ps2_keyboard_task");

  mk_thread_create(&disk_rw_test, "disk_rw_test");
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
