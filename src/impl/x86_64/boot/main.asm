global start
extern long_mode_start

section .boot.text

bits 32 ; 32 bit mode!!

start:
  ; set the stack pointer to the top of the stack
  mov esp, stack_top

  ; we want to set up 64 bit mode

  ; checks for long mode
  call check_multiboot
  call check_cpuid
  call check_long_mode

  ; we need virtual memory for long mode 
  ; set up very simple paging
  call setup_page_tables
  call enable_paging

  ; load the global descriptor table
  lgdt [gdt64.pointer]
  jmp gdt64.code_segment:long_mode_start_trampoline

  hlt

check_multiboot:
  ; magic bytes for multiboot
  cmp eax, 0x36d76289
  jne no_multiboot
  ret
no_multiboot:
  mov al, "M"
  jmp error

check_cpuid:
  ; all this does is it will try to flip the ID bit of the 
  ; flags register, then it will check if the flip was successful
  ; if not, then error
  pushfd
  pop eax

  ; store value of current flag register in ecx, then flip bit
  mov ecx, eax
  xor eax, 1<<21
  push eax

  ; check to see if the bit flip stayed by setting 
  ; flag register then checking
  popfd
  pushfd
  pop eax

  ; set it back to how it was
  push ecx
  popfd

  ; compare
  cmp eax, ecx
  je no_cpuid

  ret
no_cpuid:
  mov al, "C"
  jmp error

check_long_mode:
  ; check to see if cpu supports extended processor info
  ; this is the magic value to check
  ; cpuid takes in eax as an argument and then returns to eax
  mov eax, 0x80000000
  cpuid
  cmp eax, 0x80000001
  jb no_long_mode

  ; check long mode
  ; this time it will store the answer in edx
  mov eax, 0x80000001
  cpuid
  test edx, 1 << 29
  jz no_long_mode

  ret
no_long_mode:
  mov al, "L"
  jmp error

setup_page_tables:
  mov eax, page_table_l3
  or eax, 0b11 ; present, writeable flags
  mov [page_table_l4], eax

  ; just playign aroung with the MMU, hopefully i remmeber to delete
  mov eax, page_table_l3
  or eax, 0b11 ; present, writeable flags
  mov [page_table_l4 + 511 * 8], eax

  mov eax, page_table_l2
  or eax, 0b11 ; present, writeable flags
  mov [page_table_l3 + 0 * 8], eax

  mov eax, page_table_l2
  or eax, 0b11 ; present, writeable flags
  mov [page_table_l3 + 510 * 8], eax


  mov ecx, 0

.fill_loop:

  mov eax, 0x200000 ; 2MB
  mul ecx
  or eax, 0b10000011 ; present, writeable, huge page flags
  mov [page_table_l2 + ecx * 8], eax

  inc ecx
  ; map out 32MB for the kernel from paddr 0x0-0x1ffffff
  ; cmp ecx, 16

  ; map out more cuz who cares
  cmp ecx, 512
  jne .fill_loop
  
  ret

enable_paging:
  ; pass page table location to cpu
  mov eax, page_table_l4
  mov cr3, eax

  ; enable PAE
  mov eax, cr4
  or eax, 1 << 5
  mov cr4, eax

  ; enable long mode by writing to msr register
  mov ecx, 0xc0000080
  rdmsr
  or eax, 1 << 8
  wrmsr

  ; enable paging
  mov eax, cr0
  or eax, 1 << 31
  mov cr0, eax

  ret

error:
  mov dword [0xb8000], 0x4f524f45
  mov dword [0xb8004], 0x4f3a4f52
  mov dword [0xb8008], 0x4f204f20
  mov byte [0xb800a], al

  hlt
    
bits 64
long_mode_start_trampoline:
  ; now in 64bit mode, jump to high virtual address
  mov rax, long_mode_start
  jmp rax

section .boot.bss
align 0x1000

; page tables setup
page_table_l4:
  resb 0x1000
page_table_l3:
  resb 0x1000
page_table_l2:
  resb 0x1000
page_table_l1:
  resb 0x1000

; reserve memory space for the stack
stack_bottom:
  resb 0x4000
stack_top:

section .boot.rodata

; global descriptor table is required for 64bit mode
gdt64:
  dq 0 ; zero entry
.code_segment: equ $ - gdt64
  ; this is the kernel code segment descriptor
  ; executable flag, descriptor type flag
  ; present flag, 64bit flag
  dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53) 

  ; 0x10: kernel data segment descriptor
  ; type=data (0x2), S=1, P=1, no 64-bit flag
  dq (1 << 41) | (1 << 44) | (1 << 47)
.pointer:
  ; size
  dw $ - gdt64 - 1
  ; ptr
  dq gdt64
