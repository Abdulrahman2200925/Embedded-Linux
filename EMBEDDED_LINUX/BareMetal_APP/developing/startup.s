.section .text.boot // special section name so linker script can place it at the first in binary
.global _start // Entry point for the linker

_start:
    ldr x30, =0x10000000
    mov sp, x30
    bl main

hang:
    b hang
