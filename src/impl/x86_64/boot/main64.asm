global long_mode_start
extern kernel_main

section .text
bits 64

long_mode_start:
  ; clear
  mov ax, 0
  mov ss, ax
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  ; print "OK" to the screen
  ; the "video memory" starts at address 0xb8000
  ; mov dword [0xb8000], 0x2f4b2f4f

  mov rsp, virt_stack_top

  call kernel_main

  hlt

virt_stack_bottom:
  resb 0x4000
virt_stack_top: