#include "memlayout.h"

.equ MEM_REMAP_CR, (MEM_CONTROLLER_UI + 0x00)
.equ ZERO, 0x0
.equ IRQ_ADDR, 0x0018
.equ REMAP_CHECK_REGISTER, (SRAM_ZERO + 0x20)
.equ STACK_START, 0x002FFFF8            @ 8 byte aligned stack start (just before 0x0030_0000)

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

@ expects r0 to be the address
_read_addr:
        stmfd sp!, {r0, lr}
        ldr r1, [r0]
        bl _print_mem
        ldmfd sp!, {r0, lr}
        bx lr

.global _Lend
_Lend:
        b .
