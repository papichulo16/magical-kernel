#include "context.h"
#include "thread.h"
#include "print.h"
#include "inlines.c"
#include "pic.h"
#include "isr.h"
#include "mklib.h"
#include "cmd.h"

#include <stdint.h>

bool shift = false;

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

char cmd_buf[256];
uint8_t cmd_pos = 0;

void __dump_registers(isr_xframe_t* frame) {
    char itoa_buffer[67];
    print_str       ("\r\n\r\nprocess register dump:\r\n\trax: ");
    print_qword     ((uint64_t)frame->general_registers.rax);
    print_str       (", rbx: ");
    print_qword     ((uint64_t)frame->general_registers.rbx);
    print_str       (", rcx: ");
    print_qword     ((uint64_t)frame->general_registers.rcx);
    print_str       (", rdx: ");
    print_qword     ((uint64_t)frame->general_registers.rdx);
    print_str       (", rdi: ");
    print_qword     ((uint64_t)frame->general_registers.rdi);
    print_str       (", rsi: ");
    print_qword     ((uint64_t)frame->general_registers.rsi);
    print_str       ("\r\n\tr8: ");
    print_qword     ((uint64_t)frame->general_registers.r8);
    print_str       (", r9: ");
    print_qword     ((uint64_t)frame->general_registers.r9);
    print_str       (", r10: ");
    print_qword     ((uint64_t)frame->general_registers.r10);
    print_str       (", r11: ");
    print_qword     ((uint64_t)frame->general_registers.r11);
    print_str       ("\r\n\tr12: ");
    print_qword     ((uint64_t)frame->general_registers.r12);
    print_str       (", r13: ");
    print_qword     ((uint64_t)frame->general_registers.r13);
    print_str       (", r14: ");
    print_qword     ((uint64_t)frame->general_registers.r14);
    print_str       (", r15: ");
    print_qword     ((uint64_t)frame->general_registers.r15);
    print_str       ("\r\n\tcr0: ");
    print_qword     ((uint64_t)frame->control_registers.cr0);
    print_str       (", cr2: ");
    print_qword     ((uint64_t)frame->control_registers.cr2);
    print_str       (", cr3: ");
    print_qword     ((uint64_t)frame->control_registers.cr3);
    print_str       (", cr4: ");
    print_qword     ((uint64_t)frame->control_registers.cr4);
    print_str       ("\r\n\trsp: ");
    print_qword     ((uint64_t)frame->base_frame.rsp);
    print_str       (", rbp: ");
    print_qword     ((uint64_t)frame->base_frame.rbp);
    print_str       (", rflags: ");
    print_qword     ((uint64_t)frame->base_frame.rflags);
    print_str       ("\r\n\terror code: b*");
    print_str       ("\r\n");
}

void halt() {
    while (true) {
        __asm__ volatile ("cli; hlt");
    }
}

void mk_exception_handler(isr_xframe_t* frame) {
    print_error("\nFATAL ");
    print_byte(frame->base_frame.vector);
    print_error(__exception_labels[frame->base_frame.vector]);
    print_error(":\r\n");

    __dump_registers(frame);

    halt();
}

struct regs_context tmp;
struct mk_thread_obj* mk_working_thread;

void mk_timer_int_handler(uint64_t* stack) {
    
    mk_working_thread = mk_get_working_thread();
    
    if (mk_working_thread->time_slice > 0) {
        mk_working_thread->time_slice -= 1;

        mk_pic_send_eoi(0);

        return;
    }
        
    if (!mk_working_thread->started) {
        mk_pic_send_eoi(0);
        mk_working_thread->started = 1;

        mk_thread_ctx_restore_from_stack(&mk_working_thread->regs, stack);

        return;
    }
    
    mk_thread_ctx_save_from_stack(&mk_working_thread->regs, stack);

    mk_pic_send_eoi(0);

    if (!mk_thread_ctx_switch()) {
        mk_working_thread = mk_get_working_thread();

        mk_thread_ctx_restore_from_stack(&mk_working_thread->regs, stack);
    }
}

void mk_keyboard_int_handler() {
    // status register bit 1 = output  buffer status
    if ((inb(0x64) & 1) == 0)
        return;
    
    uint8_t scancode = inb(0x60); // read the data port
    
    // ignore key releases
    if (scancode & 0x80) {
        scancode &= 0x7F;
        if (scancode == 0x2A || scancode == 0x36)
            shift = false;
    }
    else {
        if (scancode == 0x2A || scancode == 0x36) {
            shift = true;
            mk_pic_send_eoi(1);

            return;
        }
        
        char c = translate_scancode_set_1(scancode, shift);
        
        if (c == '\n') {
            print_char(c);

            mk_handle_cmd((char *) &cmd_buf);
            memset(&cmd_buf, 0, sizeof(cmd_buf));
            cmd_pos = 0;

            print_str("$ ");
            mk_pic_send_eoi(1);

            return;
        }

        if (cmd_pos == 255) {
            mk_pic_send_eoi(1);

            return;
        }

        cmd_buf[cmd_pos++] = c;
        print_char(c);
    }

    mk_pic_send_eoi(1);
}
