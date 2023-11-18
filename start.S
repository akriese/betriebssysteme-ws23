.section .init
.global _start
.global _perform_reset
.global _perform_int

.equ MEM_CONTROLLER_UI, 0xFFFFFF00
.equ MEM_REMAP_CR, (MEM_CONTROLLER_UI + 0x00)
.equ ZERO, 0x0
.equ SRAM_ZERO, 0x00200000
.equ IRQ_ADDR, 0x0018
.equ SOME_RANDOM_ADDR, 0x44

_start:
        b reset_handler
        b undef_instr_handler
        b sw_int_handler
        b prefetch_abt_handler
        b data_abt_handler
        b _Lend
        b irq_handler
        b fiq_handler

reset_handler:
        mov r0, #0
        bl _pr_exception

        @ ldr r0, =ZERO
        @ bl _read_addr

        bl _remap_if_needed

        ldr r0, =ZERO
        bl _read_addr

        ldr r0, =SOME_RANDOM_ADDR
        bl _read_addr

        stmfd sp!, {r0, r1, lr}
        bl main
        ldmfd sp!, {r0, r1, lr}
        b _Lend

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
        bx lr

_perform_int:
        b IRQ_ADDR
        bx lr

_remap_if_needed:
        @ somehow this leads to an unknown error, so its commented out for now
        @ ldr r0, =ZERO
        @ ldr r1, [r0]
        @ ldr r0, =SRAM_ZERO
        @ ldr r2, [r0]

        @ cmp r1, r2
        @ beq .end

        .remap:
        ldr r0, =MEM_REMAP_CR   @ define register with hardcoded address
        mov r1, #1              @
        str r1, [r0]

        .end:
        bx lr                   @ return

@ expects r0 to be the address
_read_addr:
        stmfd sp!, {r0, r1, lr}
        ldr r1, [r0]
        bl _print_mem
        ldmfd sp!, {r0, r1, lr}
        bx lr

_Lend:
        b .
