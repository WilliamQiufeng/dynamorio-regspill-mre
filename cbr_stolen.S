    .text
    .global _start
    .type _start, %function
_start:
    mov     x0, x28
    b _start
