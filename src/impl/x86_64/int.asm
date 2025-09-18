global mk_isr_stub_table
global mk_asm_keyboard_int
global mk_asm_timer_int

extern mk_exception_handler
extern mk_timer_int_handler
extern mk_keyboard_int_handler

section .text
bits 64

; macros for exception handlers
%macro isr_err_stub 1
isr_stub_%+%1:
    call mk_exception_handler
    iretq
%endmacro

%macro isr_no_err_stub 1
isr_stub_%+%1:
    call mk_exception_handler
    iretq
%endmacro

; timer interrupt
mk_asm_timer_int:
    call mk_timer_int_handler
    iretq

; keyboard interrupt
mk_asm_keyboard_int:
    call mk_keyboard_int_handler
    iretq

; exception table
isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31

; create the isr stub table
section .rodata
mk_isr_stub_table:
  %assign i 0 
  %rep    32 
      dq isr_stub_%+i
  %assign i i+1 
  %endrep

