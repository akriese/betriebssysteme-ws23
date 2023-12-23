.extern get_banked_registers
.extern set_banked_registers

.global _ivt_start
_ivt_start:
        ldr pc, jt_unhandled
        ldr pc, jt_undef
        ldr pc, jt_swi
        ldr pc, jt_prefabort
        ldr pc, jt_dataabort
        ldr pc, jt_unhandled
        ldr pc, [pc, #-0xF20]
        ldr pc, jt_fiq

jt_undef:       .word undef_trampoline
jt_swi:         .word swi_trampoline
jt_prefabort:   .word prefabort_trampoline
jt_dataabort:   .word dataabort_trampoline
jt_irq:         .word irq_trampoline
jt_fiq:         .word fiq_trampoline
jt_unhandled:   .word unhandled_trampoline

.global _ivt_end
_ivt_end:

unhandled_trampoline:
        b _Lend

undef_trampoline:
        /* save registers to stack */
        stmdb sp, {r0-r15}
        sub sp, sp, #(16*4)

        mov r0, sp
        bl undef_handler

        ldmfd sp!, {r0-r12}
        add sp, #4

        /* jump back to address after bad instruction */
        ldmfd sp!, {pc}^

        b _Lend

swi_trampoline:
        /* save important registers; IRQ mode lr is the usr mode's pc */
        stmfd sp!, {r0-r12, lr}

        @ load the causing swi instruction to retrieve the swi number
        ldr r8, [lr,#-4]
        bic r8, r8, #0xFF000000 @ only use the last 3 bytes of the instruction

        /* disable interrupts */
        @bl cpsr_disable_interrupts

        @ save saved psr
        mrs r4, spsr
        /* load saved pc to r2 */
        ldr r2, [sp, #(13*4)]
        @sub r2, #8

        /* push pc, cpsr */
        stmfd sp!, {r2, r4}

        mov r0, #0x1f
        bl get_banked_registers

        /* Stack looks like: (left is at higher address)
        pc, r12, r11, r10, r9, r8, r7, r6, r5, r4, r3, r2, r1, r0, cpsr, pc, lr, sp*/

        mov r0, r8 @ swi number
        mov r1, sp @ context pointer
        bl swi_handler

        /* Stack looks like: (left is at higher address)
        pc, r12, r11, r10, r9, r8, r7, r6, r5, r4, r3, r2, r1, r0, cpsr, pc, lr, sp*/

        /* re-enable interrupts */
        bl cpsr_enable_interrupts

        mov r0, #0x1f
        bl set_banked_registers

        @ load pc and cpsr
        ldmfd sp!, {r0, r1}

        msr spsr_c, r1
        mov lr, r0

        /* restore saved registers */
        ldmfd sp!, {r0-r12}

        movs pc, lr



prefabort_trampoline:
        /* save registers to stack */
        stmdb sp, {r0-r15}
        sub sp, sp, #(16*4)

        mov r0, sp
        bl prefetch_abort_handler

        b _Lend

dataabort_trampoline:
        /* save registers to stack */
        stmdb sp, {r0-r15}
        sub sp, sp, #(16*4)

        mov r0, sp
        bl data_abort_handler

        b _Lend

irq_trampoline:
        /* save registers to stack */
        stmdb sp, {r0-r15}
        sub sp, sp, #(16*4)

        mov r0, sp
        bl irq_handler

        b _Lend

fiq_trampoline:
        /* save registers to stack */
        stmdb sp, {r0-r15}
        sub sp, sp, #(16*4)

        mov r0, sp
        bl fiq_handler

        b _Lend
