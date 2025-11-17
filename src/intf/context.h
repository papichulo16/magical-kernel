#pragma once 

#include <stdint.h>

struct regs_context {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rsi, rdi, rbp, rdx, rcx, rbx, rax;
    uint64_t rip, cs, rflags, rsp, ss;
};

void mk_thread_ctx_save_from_stack(struct regs_context* ctx, uint64_t* stack);
void mk_thread_ctx_restore_from_stack(struct regs_context* ctx, uint64_t* stack);
