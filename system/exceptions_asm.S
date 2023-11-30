.global _ivt_start
_ivt_start:
        b jt_unhandled
        b jt_undef
        b jt_swi
        b jt_prefabort
        b jt_dataabort
        b jt_unhandled
        b jt_irq
        b jt_fiq

jt_undef:       .word undef_trampoline
jt_swi:         .word swi_trampoline
jt_prefabort:   .word prefabort_trampoline
jt_dataabort:   .word dataabort_trampoline
jt_irq:         .word irq_trampoline
jt_fiq:         .word fiq_trampoline
jt_unhandled:   .word unhandled_trampoline

.global _ivt_end
_ivt_end:
