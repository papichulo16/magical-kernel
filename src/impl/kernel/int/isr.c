#include "isr.h"
#include "cmd.h"
#include "context.h"
#include "inlines.c"
#include "mklib.h"
#include "pic.h"
#include "print.h"
#include "thread.h"
#include "slab.h"
#include "virt.h"
#include "drivers.h"

#include <stdint.h>

#define SCHED_DEBUG 0

char* __exception_labels[] = {
      "[0x00] Divide by Zero Exception",
      "[0x01] Debug Exception",
      "[0x02] Unhandled Non-maskable Interrupt",
      "[0x03] Breakpoint Exception",
      "[0x04] Overflow Exception",
      "[0x05] Bound Range Exceeded Exception",
      "[0x06] Invalid Opcode/Operand Exception",
      "[0x07] Device Unavailable Exception",
      "[0x08] Double Fault",
      "[0x09] Coprocessor Segment Overrun",
      "[0x0A] Invalid TSS Exception",
      "[0x0B] Absent Segment Exception",
      "[0x0C] Stack-segment Fault",
      "[0x0D] General Protection Fault",
      "[0x0E] Page Fault",
      "[0x0F] Inexplicable Error",
      "[0x10] x87 Floating Exception",
      "[0x11] Alignment Check",
      "[0x12] Machine Check",
      "[0x13] SIMD Floating Exception",
      "[0x14] Virtualized Exception",
      "[0x15] Control Protection Exception",
      "[0x16] Inexplicable Error",
      "[0x17] Inexplicable Error",
      "[0x18] Inexplicable Error",
      "[0x19] Inexplicable Error",
      "[0x1A] Inexplicable Error",
      "[0x1B] Inexplicable Error",
      "[0x1C] Hypervisor Intrusion Exception",
      "[0x1D] VMM Communications Exception",
      "[0x1E] Security Exception",
      "[0x1F] Inexplicable Error"
};

void __dump_registers(struct regs_context* frame) {
  print_str("\n\nprocess register dump:\nrip: ");
  print_qword((uint64_t)frame->rip);
  print_str(", rax: ");
  print_qword((uint64_t)frame->rax);
  print_str(", rbx: ");
  print_qword((uint64_t)frame->rbx);
  print_str("\nrcx: ");
  print_qword((uint64_t)frame->rcx);
  print_str(", rdx: ");
  print_qword((uint64_t)frame->rdx);
  print_str(", rdi: ");
  print_qword((uint64_t)frame->rdi);
  print_str("\nrsi: ");
  print_qword((uint64_t)frame->rsi);
  print_str(", r8: ");
  print_qword((uint64_t)frame->r8);
  print_str(", r9: ");
  print_qword((uint64_t)frame->r9);
  print_str("\nr10: ");
  print_qword((uint64_t)frame->r10);
  print_str(", r11: ");
  print_qword((uint64_t)frame->r11);
  print_str("r12: ");
  print_qword((uint64_t)frame->r12);
  print_str("\nr13: ");
  print_qword((uint64_t)frame->r13);
  print_str(", r14: ");
  print_qword((uint64_t)frame->r14);
  print_str(", r15: ");
  print_qword((uint64_t)frame->r15);

  /*
  print_str("\r\n\tcr0: ");
  print_qword((uint64_t)frame.cr0);
  print_str(", cr2: ");
  print_qword((uint64_t)frame.cr2);
  print_str(", cr3: ");
  print_qword((uint64_t)frame.cr3);
  print_str(", cr4: ");
  print_qword((uint64_t)frame.cr4);
  */

  print_str("\nrsp: ");
  print_qword((uint64_t)frame->rsp);
  print_str(", rbp: ");
  print_qword((uint64_t)frame->rbp);
  print_str(", rflags: ");
  print_qword((uint64_t)frame->rflags);
}

void halt() {
  while (true) {
    __asm__ volatile("cli; hlt");
  }
}

void mk_exception_handler(uint64_t stack, uint64_t vector) {
  struct regs_context frame;

  mk_thread_ctx_save_from_stack(&frame, (uint64_t *) stack);

  //print_user_clear();

  print_error("\nFATAL ");
  print_error(__exception_labels[vector & 0x1f]);
  print_error(":\n");

  __dump_registers(&frame);

  halt();
}

struct regs_context tmp;
struct mk_thread_obj* mk_working_thread;
struct mk_sema_t* sema_sig = 0;

void t_res_state(uint64_t* stack) {
    mk_thread_ctx_restore_from_stack(&mk_working_thread->regs, stack);

    if (SCHED_DEBUG) {
      print_str("[*] isr.c: ");
      print_str(mk_working_thread->thread_name);
      print_str(" ctx restore rip ");
      print_qword(mk_working_thread->regs.rip);
      print_str(", rsp ");
      print_qword(mk_working_thread->regs.rsp);
      print_char('\n');
    }
}

void t_init(uint64_t* stack) {
    mk_working_thread->started = 1;
    mk_working_thread->state = MK_THREAD_WORKING;
    mk_working_thread->time_slice = THREAD_TIME_SLICE;

    mk_thread_ctx_restore_from_stack(&mk_working_thread->regs, stack);
}

void t_save(uint64_t* stack) {
    mk_thread_ctx_save_from_stack(&mk_working_thread->regs, stack);

    if (SCHED_DEBUG) {
      print_str("[*] isr.c: ");
      print_str(mk_working_thread->thread_name);
      print_str(" ctx save rip ");
      print_qword(mk_working_thread->regs.rip);
      print_str(", rsp ");
      print_qword(mk_working_thread->regs.rsp);
      print_char('\n');
    }
}

int t_dis_by_state(uint64_t* stack) {

  switch (mk_working_thread->state) {

    case MK_THREAD_KILLED:
	break;

    case MK_THREAD_WORKING:
	t_save(stack);

    	break;

    case MK_THREAD_SLEEPING:
	t_save(stack);

	if(sema_sig)
		mk_sema_enq(sema_sig, mk_working_thread);

	sema_sig = 0;

    	break;

    default:
	return 1;
  }

  return 0;
}

void mk_sema_sig_set(struct mk_sema_t* sema) {
  sema_sig = sema;
}

void mk_timer_int_handler(uint64_t* stack) {
  disable_interrupts();

  _mk_timer_int_handler(stack);

  mk_pic_send_eoi(0);
  enable_interrupts();
}

int first_thread = 0;

void _mk_timer_int_handler(uint64_t* stack) {

  mk_working_thread = mk_get_working_thread();

  if (mk_working_thread->time_slice > 0) {

    mk_working_thread->time_slice--;
    return;
  }

  if (!first_thread) {

    t_init(stack);
    first_thread = 1;

    return;
  }

  if (!t_dis_by_state(stack)) {
      mk_thread_ctx_switch();

      mk_working_thread = mk_get_working_thread();
      t_res_state(stack);
  }

}

void mk_keyboard_int_handler() {
  mk_sema_give(keyboard_sema);
  mk_pic_send_eoi(1);
}

