#include "context.h"

void mk_thread_ctx_save(struct regs_context* ctx) {

    asm volatile (
        ".intel_syntax noprefix\n\t"
        "push rax\n\t"  

        "mov rax, qword ptr [rsp + 0xa0]\n\t"  // r15
        "mov qword ptr [rdi + 0x00], rax\n\t"

        "mov rax, qword ptr [rsp + 0x98]\n\t"  // r14
        "mov qword ptr [rdi + 0x08], rax\n\t"

        "mov rax, qword ptr [rsp + 0x90]\n\t"  // r13
        "mov qword ptr [rdi + 0x10], rax\n\t"

        "mov rax, qword ptr [rsp + 0x88]\n\t"  // r12
        "mov qword ptr [rdi + 0x18], rax\n\t"

        "mov rax, qword ptr [rsp + 0x80]\n\t"  // r11
        "mov qword ptr [rdi + 0x20], rax\n\t"

        "mov rax, qword ptr [rsp + 0x78]\n\t"  // r10
        "mov qword ptr [rdi + 0x28], rax\n\t"

        "mov rax, qword ptr [rsp + 0x70]\n\t"  // r9
        "mov qword ptr [rdi + 0x30], rax\n\t"

        "mov rax, qword ptr [rsp + 0x68]\n\t"  // r8
        "mov qword ptr [rdi + 0x38], rax\n\t"

        "mov rax, qword ptr [rsp + 0x60]\n\t"  // rsi
        "mov qword ptr [rdi + 0x40], rax\n\t"

        "mov rax, qword ptr [rsp + 0x58]\n\t"  // rdi
        "mov qword ptr [rdi + 0x48], rax\n\t"

        "mov rax, qword ptr [rsp + 0x50]\n\t"  // rbp
        "mov qword ptr [rdi + 0x50], rax\n\t"

        "mov rax, qword ptr [rsp + 0x48]\n\t"  // rdx
        "mov qword ptr [rdi + 0x58], rax\n\t"

        "mov rax, qword ptr [rsp + 0x40]\n\t"  // rcx
        "mov qword ptr [rdi + 0x60], rax\n\t"

        "mov rax, qword ptr [rsp + 0x38]\n\t"  // rbx
        "mov qword ptr [rdi + 0x68], rax\n\t"

        "mov rax, qword ptr [rsp + 0x28]\n\t"  // rsp
        "mov qword ptr [rdi + 0x90], rax\n\t"

        "mov rax, qword ptr [rsp + 0xa8]\n\t"  // rip
        "mov qword ptr [rdi + 0x78], rax\n\t"

        "mov rax, qword ptr [rsp + 0xb0]\n\t"  // cs
        "mov qword ptr [rdi + 0x80], rax\n\t"

        "mov rax, qword ptr [rsp + 0xb8]\n\t"  // rflags
        "mov qword ptr [rdi + 0x88], rax\n\t"

        "pop rax\n\t"  // rax
        "mov qword ptr [rdi + 0x70], rax\n\t"

       ".att_syntax prefix\n\t"
        :
        : "D"(ctx)
        : "rax", "memory"
    );
}

void mk_thread_ctx_restore(struct regs_context* ctx) {
    asm volatile (
        ".intel_syntax noprefix\n\t"

        "mov r15, [rdi + 0x00]\n\t"
        "mov r14, [rdi + 0x08]\n\t"
        "mov r13, [rdi + 0x10]\n\t"
        "mov r12, [rdi + 0x18]\n\t"
        "mov r11, [rdi + 0x20]\n\t"
        "mov r10, [rdi + 0x28]\n\t"
        "mov r9, [rdi + 0x30]\n\t"
        "mov r8, [rdi + 0x38]\n\t"
        "mov rsi, [rdi + 0x40]\n\t"
        "mov rbp, [rdi + 0x50]\n\t"
        "mov rdx, [rdi + 0x58]\n\t"
        "mov rcx, [rdi + 0x60]\n\t"
        "mov rbx, [rdi + 0x68]\n\t"

        "mov rsp, [rdi + 0x90]\n\t"
        "mov rax, [rdi + 0x90]\n\t"
        "push rax\n\t"
        "mov rax, [rdi + 0x88]\n\t"
        "push rax\n\t"
        "mov rax, [rdi + 0x80]\n\t"
        "push rax\n\t"
        "mov rax, [rdi + 0x78]\n\t"
        "push rax\n\t"

        "mov rax, [rdi + 0x70]\n\t"
        "mov rdi, [rdi + 0x48]\n\t"
        "iretq\n\t"
        ".att_syntax prefix\n\t"
        :
        : "D"(ctx)
        : "rax", "memory"
    );
}

