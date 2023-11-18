.section .init
.global _start
.global _perform_reset
.global _perform_int

.equ MEM_CONTROLLER_UI, 0xFFFFFF00
.equ MEM_REMAP_CR, (MEM_CONTROLLER_UI + 0x00)
.equ ZERO, 0x0
.equ IRQ_ADDR, 0x0018

_start:
        b reset_handler
        b undef_instr_handler
        b sw_int_handler
        b prefetch_abt_handler
        b data_abt_handler
        b .
        b irq_handler
        b fiq_handler

reset_handler:
        bl _remap
        stmfd sp!, {r1, lr}
        b main
        ldmfd sp!, {r1, lr}

undef_instr_handler:
        mov r0, #1
        bl _pr_exception
        b _Lend

sw_int_handler:
        mov r0, #2
        bl _pr_exception
        b _Lend

prefetch_abt_handler:
        mov r0, #3
        bl _pr_exception
        b _Lend

data_abt_handler:
        mov r0, #4
        bl _pr_exception
        b _Lend

irq_handler:
        mov r0, #5
        bl _pr_exception
        b _Lend

fiq_handler:
        mov r0, #6
        bl _pr_exception
        b _Lend

_pr_exception:
        stmfd sp!, {r1, lr}
        bl _print_exception
        ldmfd sp!, {r1, lr}
        bx lr

_perform_reset:
        b ZERO

_perform_int:
        b IRQ_ADDR

_start_bak:
        @ simply execute the main OS function
        bl main

        @ infinite loop
        b .

_remap:
        stmfd sp!, {r1, lr}
        bl test_print
        ldmfd sp!, {r1, lr}
        ldr r0, =MEM_REMAP_CR   @ define register with hardcoded address
        mov r1, #1              @
        str r1, [r0]
        bx lr                   @ return

