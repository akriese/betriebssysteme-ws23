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

undef_trampoline:
        /* Debug-Registersatz sichern */
        stmdb sp!, {r0-r15}
        /*sub sp, sp, #(16*4) */

        /* C-Handler (ohne Wiederkehr) aufrufen mit Zeiger auf Registersatz */
        mov r0, #1
        bl _print_exception
        @ mov r0, sp
        @ b register_dump
        b _Lend

swi_trampoline:
        mov r0, #2
        bl _pr_exception
        b _Lend

prefabort_trampoline:
        mov r0, #3
        bl _pr_exception
        b _Lend

dataabort_trampoline:
        mov r0, #4
        bl _pr_exception
        b _Lend

irq_trampoline:
        mov r0, #5
        bl _pr_exception
        b _Lend

fiq_trampoline:
        mov r0, #6
        bl _pr_exception
        b _Lend

_pr_exception:
        push {r1, lr}
        bl _print_exception
        pop {r1, lr}
        bx lr
