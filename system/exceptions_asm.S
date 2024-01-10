.extern add_context_to_stack
.extern restore_context_from_stack

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

/**
* Software interrupt handler
*
* This function saves the current context (registers, cpsr), extracts the SWI number
* and executes the C SWI handler.
* The handler might change the context (e.g. thread switch).
* Afterwards, the context is restored to proceed with the usual execution.
*/
swi_trampoline:
        push {lr}
        bl add_context_to_stack

        @Stack after call: lr(user mode pc), lr, cpsr, pc, lr, sp, r12, r11,..., r1, r0(<-)

        @ retrieve user mode pc
        ldr lr, [sp, #(15*4)]

        @ load the causing swi instruction to retrieve the swi number
        ldr r0, [lr, #-4]
        bic r0, r0, #0xFF000000 @ only use the last 3 bytes of the instruction
        mov r1, sp @ context pointer
        bl swi_handler

        b restore_context_from_stack

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
