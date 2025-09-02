; magic data stuff needed to start boot

section .multiboot_header
_start_header:
  ; magic number
  dd 0xe85250d6 ; multiboot2

  ; arch
  dd 0 ; protected mode

  ; header len
  dd _end_header - _start_header

  ; checksum
  dd 0x100000000 - (0xe85250d6 + (_end_header - _start_header))

  ; end tag
  dw 0
  dw 0
  dd 0

_end_header:
