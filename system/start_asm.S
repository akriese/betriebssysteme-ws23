#include "memlayout.h"

.global _start
_start:
        ldr sp, =STACK_BOTTOM

        bl setup_kernel

        stmfd sp!, {r0 - r15}
        mov r0, sp
        bl register_dump
        add sp, #64

        bl main

        b _Lend

.global _Lend
_Lend:
        b .
