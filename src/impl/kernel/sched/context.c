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
    
    // Interrupt frame
    ctx->rip    = stack[15];
    ctx->cs     = stack[16];
    ctx->rflags = stack[17];

    // CS & 3 gives us the RPL (requested privilege level)
    if ((ctx->cs & 0x3) != 0) {
        ctx->rsp = stack[18];
        ctx->ss  = stack[19];
    } else {
        ctx->rsp = (uint64_t)(&stack[15]);
        
        uint64_t current_ss;
        asm volatile("mov %%ss, %0" : "=r"(current_ss));
        ctx->ss = current_ss;
    }
}

void mk_thread_ctx_restore_from_stack(struct regs_context* ctx, uint64_t* stack) {
    asm volatile (
        ".intel_syntax noprefix\n\t"
        
        // rsi = stack pointer passed in
        // rdi = ctx pointer
        
        // Overwrite the interrupt frame on the stack with new values
        "mov rax, [rdi + 0x78]\n\t"   // RIP
        "mov [rsi + 0x78], rax\n\t"   // stack[15]
        
        "mov rax, [rdi + 0x80]\n\t"   // CS
        "mov [rsi + 0x80], rax\n\t"   // stack[16]
        
        "mov rax, [rdi + 0x88]\n\t"   // RFLAGS
        "mov [rsi + 0x88], rax\n\t"   // stack[17]
        
        "mov rax, [rdi + 0x90]\n\t"   // RSP
        "mov [rsi + 0x90], rax\n\t"   // stack[18]
        
        "mov rax, [rdi + 0x98]\n\t"   // SS
        "mov [rsi + 0x98], rax\n\t"   // stack[19]
        
        // Overwrite the saved registers on the stack
        "mov rax, [rdi + 0x70]\n\t"   // rax
        "mov [rsi + 0x00], rax\n\t"
        
        "mov rax, [rdi + 0x68]\n\t"   // rbx
        "mov [rsi + 0x08], rax\n\t"
        
        "mov rax, [rdi + 0x60]\n\t"   // rcx
        "mov [rsi + 0x10], rax\n\t"
        
        "mov rax, [rdi + 0x58]\n\t"   // rdx
        "mov [rsi + 0x18], rax\n\t"
        
        "mov rax, [rdi + 0x50]\n\t"   // rbp
        "mov [rsi + 0x20], rax\n\t"
        
        "mov rax, [rdi + 0x48]\n\t"   // rdi (save for last)
        "mov [rsi + 0x28], rax\n\t"
        
        "mov rax, [rdi + 0x40]\n\t"   // rsi
        "mov [rsi + 0x30], rax\n\t"
        
        "mov rax, [rdi + 0x38]\n\t"   // r8
        "mov [rsi + 0x38], rax\n\t"
        
        "mov rax, [rdi + 0x30]\n\t"   // r9
        "mov [rsi + 0x40], rax\n\t"
        
        "mov rax, [rdi + 0x28]\n\t"   // r10
        "mov [rsi + 0x48], rax\n\t"
        
        "mov rax, [rdi + 0x20]\n\t"   // r11
        "mov [rsi + 0x50], rax\n\t"
        
        "mov rax, [rdi + 0x18]\n\t"   // r12
        "mov [rsi + 0x58], rax\n\t"
        
        "mov rax, [rdi + 0x10]\n\t"   // r13
        "mov [rsi + 0x60], rax\n\t"
        
        "mov rax, [rdi + 0x08]\n\t"   // r14
        "mov [rsi + 0x68], rax\n\t"
        
        "mov rax, [rdi + 0x00]\n\t"   // r15
        "mov [rsi + 0x70], rax\n\t"
        
        ".att_syntax prefix\n\t"
        :
        : "D"(ctx), "S"(stack)
        : "rax", "memory"
    );
}

