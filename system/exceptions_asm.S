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

/**
* Restores the context of a thread which currenly lies on the stack.
*
* The stack is assumed to look like: (left is at higher address)
* pc, r12, r11, r10, r9, r8, r7, r6, r5, r4, r3, r2, r1, r0, cpsr, pc, lr, sp
*
* At the end, lr is moved to pc to resume with the normal execution
*/
.global restore_context
restore_context:

        mov r0, #0x1f
        bl set_banked_registers

        @ load pc and cpsr
        ldmfd sp!, {r0, r1}

        msr spsr_c, r1
        mov lr, r0

        /* restore saved registers */
        ldmfd sp!, {r0-r12}

        movs pc, lr

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

/**
* Software interrupt handler
*
* This function saves the current context (registers, cpsr), extracts the SWI number
* and executes the C SWI handler.
* The handler might change the context (e.g. thread switch).
* Afterwards, the context is restored to proceed with the usual execution.
*/
swi_trampoline:
        /* save important registers; IRQ mode lr is the usr mode's pc */
        stmfd sp!, {r0-r12}

        /* disable interrupts */
        @bl cpsr_disable_interrupts

        @ save saved psr
        mrs r1, spsr
        /* load saved pc to r0 */
        mov r0, lr

        /* push pc (lr), cpsr */
        stmfd sp!, {r0, r1}

        mov r0, #0x1f
        bl get_banked_registers

        @ retrieve lr; 2 ints (banked regsiters) have been written since lr was pushed
        ldr lr, [sp, #8]

        @ load the causing swi instruction to retrieve the swi number
        ldr r0, [lr, #-4]
        bic r0, r0, #0xFF000000 @ only use the last 3 bytes of the instruction
        mov r1, sp @ context pointer
        bl swi_handler

        b restore_context

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
