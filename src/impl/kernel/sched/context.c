#include "context.h"

void mk_thread_ctx_save_from_stack(struct regs_context* ctx, uint64_t* stack) {
    ctx->rax = stack[0];
    ctx->rbx = stack[1];
    ctx->rcx = stack[2];
    ctx->rdx = stack[3];
    ctx->rbp = stack[4];
    ctx->rdi = stack[5];
    ctx->rsi = stack[6];
    ctx->r8  = stack[7];
    ctx->r9  = stack[8];
    ctx->r10 = stack[9];
    ctx->r11 = stack[10];
    ctx->r12 = stack[11];
    ctx->r13 = stack[12];
    ctx->r14 = stack[13];
    ctx->r15 = stack[14];
    
    ctx->rip    = stack[15];
    ctx->cs     = stack[16];
    ctx->rflags = stack[17];
    ctx->rsp = stack[18];
    ctx->ss  = stack[19];
}

void mk_thread_ctx_restore_from_stack(struct regs_context* ctx, uint64_t* stack) {
    stack[0] = ctx->rax;
    stack[1] = ctx->rbx;
    stack[2] = ctx->rcx;
    stack[3] = ctx->rdx;
    stack[4] = ctx->rbp;
    stack[5] = ctx->rdi;
    stack[6] = ctx->rsi;
    stack[7] = ctx->r8;
    stack[8] = ctx->r9;
    stack[9] = ctx->r10;
    stack[10] = ctx->r11;
    stack[11] = ctx->r12;
    stack[12] = ctx->r13;
    stack[13] = ctx->r14;
    stack[14] = ctx->r15;
    
    stack[15] = ctx->rip;
    stack[16] = ctx->cs;
    stack[17] = ctx->rflags;
    stack[18] = ctx->rsp;
    stack[19] = ctx->ss;
}

