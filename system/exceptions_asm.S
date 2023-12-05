.global _ivt_start
_ivt_start:
        ldr pc, jt_unhandled
        ldr pc, jt_undef
        ldr pc, jt_swi
        ldr pc, jt_prefabort
        ldr pc, jt_dataabort
        ldr pc, jt_unhandled
        ldr pc, jt_irq
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
        /* save registers to stack */
        stmdb sp, {r0-r15}
        sub sp, sp, #(16*4)

        mov r0, sp
        bl swi_handler

        b _Lend

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
