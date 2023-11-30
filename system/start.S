.section .init
.global _start
.global _perform_reset
.global _perform_int

.equ MEM_CONTROLLER_UI, 0xFFFFFF00
.equ MEM_REMAP_CR, (MEM_CONTROLLER_UI + 0x00)
.equ ZERO, 0x0
.equ SRAM_ZERO, 0x00200000
.equ IRQ_ADDR, 0x0018
.equ REMAP_CHECK_REGISTER, (SRAM_ZERO + 0x20)
.equ STACK_START, 0x002FFFF8            @ 8 byte aligned stack start (just before 0x0030_0000)

_start:
        b reset_handler
        b undef_instr_handler
        b sw_int_handler
        b prefetch_abt_handler
        b data_abt_handler
        b _Lend
        b irq_handler
        b fiq_handler
        .word 0x0               @ REMAP_CHECK_REGISTER: write a 1 if remap has been done

reset_handler:
        @ in this function, we dont need to save lr ever as we dont plan to jump back

        @ set the stack pointer to a fixed address; all previous information is lost
        ldr sp, =STACK_START

        mov r0, #0
        bl _pr_exception

        bl _remap_if_needed

        push {pc}
        bl _register_dump
        pop {r0}

        push {r1, lr}
        bl main
        pop {r1, lr}

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
        push {r1, lr}
        bl _print_exception
        pop {r1, lr}
        bx lr

_perform_reset:
        b ZERO
        bx lr

_perform_int:
        b IRQ_ADDR
        bx lr

_remap_if_needed:
        @ check at our custom location, if the remap already happened
        ldr r0, =REMAP_CHECK_REGISTER
        ldr r1, [r0]
        mov r2, #1

        cmp r1, r2
        beq .end

        .remap:
        ldr r0, =MEM_REMAP_CR   @ define register with hardcoded address
        mov r1, #1              @
        str r1, [r0]

        @ set custom remap flag to 1 to not accidentally remap again
        ldr r0, =REMAP_CHECK_REGISTER
        str r1, [r0]

        .end:
        bx lr                   @ return

@ expects r0 to be the address
_read_addr:
        stmfd sp!, {r0, lr}
        ldr r1, [r0]
        bl _print_mem
        ldmfd sp!, {r0, lr}
        bx lr

@ expects pc pushed in the stack by caller
_register_dump:
        @ push all registers into stack for C function to access
        push {r4 - r12, sp, lr}

        bl register_dump

        pop {r4 - r12, sp, lr}

        @ we would need to pop pc too but we dont want to overwrite pc
        @ so we would need to add 4 bytes to the sp to "forget" the pc entry
        @ this is actually done by pop being 8 byte aligned, no need to add here
        @add sp, sp, #4                          @ forget PC in stack
        bx lr

_exception_register_dump:
        push {r4 - r11, sp, lr}

        bl register_dump

        @ need to pop sp and pc without writing
        @ adding as the stack is full descending
        pop {r4 - r11, sp, lr}
        bx lr

_Lend:
        b .
