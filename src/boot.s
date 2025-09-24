MBOOT_PAGE_ALIGN EQU 1 << 0
MBOOT_MEM_INFO EQU 1 << 1
MBOOT_USE_GFX EQU 0

MBOOT_MAGIC EQU 0x1BADB002
MBOOT_FLAGS EQU MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO | MBOOT_USE_GFX
MBOOT_CHECKSUM EQU -(MBOOT_MAGIC + MBOOT_FLAGS)

section .multiboot
ALIGN 4
    DD MBOOT_MAGIC
    DD MBOOT_FLAGS
    DD MBOOT_CHECKSUM
    DD 0, 0, 0, 0, 0

    DD 0
    DD 800
    DD 600
    DD 32

SECTION .bss
ALIGN 16
stack_bottom:
    RESB 16384 * 8
stack_top:

section .boot

global _start
_start:
    MOV ecx, (initial_page_dir - 0xC0000000)
    MOV cr3, ecx

    MOV ecx, cr4
    OR ecx, 0x10
    MOV cr4, ecx

    MOV ecx, cr0
    OR ecx, 0x80000000
    MOV cr0, ecx

    JMP higher_half

section .text
higher_half:
    MOV esp, stack_top
    PUSH ebx
    PUSH eax
    XOR ebp, ebp
    extern kernel_main
    CALL kernel_main

halt:
    hlt
    JMP halt


section .data
align 4096
global initial_page_dir
initial_page_dir:
    ; Identity-map first 32 MB
    DD (0 << 22) | 10000011b   ; 0–4 MB
    DD (1 << 22) | 10000011b   ; 4–8 MB
    DD (2 << 22) | 10000011b   ; 8–12 MB
    DD (3 << 22) | 10000011b   ; 12–16 MB
    DD (4 << 22) | 10000011b   ; 16–20 MB
    DD (5 << 22) | 10000011b   ; 20–24 MB
    DD (6 << 22) | 10000011b   ; 24–28 MB
    DD (7 << 22) | 10000011b   ; 28–32 MB

    TIMES 768-8 DD 0

    ; Kernel higher-half mapping (same trick you had before)
    DD (0 << 22) | 10000011b
    DD (1 << 22) | 10000011b
    DD (2 << 22) | 10000011b
    DD (3 << 22) | 10000011b
    TIMES 256-4 DD 0
