.section .init
.global _start
.global _perform_reset
.global _perform_int

.equ MEM_CONTROLLER_UI, 0xFFFFF00
.equ MEM_REMAP_CR, (MEM_CONTROLLER_UI + 0x00)
.equ ZERO, 0x0
.equ IRQ_ADDR, 0x0018

_start:
        b reset_handler
        b undef_instr_handler
        b sw_int_handler
        b prefetch_abt_handler
        b data_abt_handler
        b irq_handler
        b fiq_handler

reset_handler:
        @ bl _remap
        b main

undef_instr_handler:
        b .

sw_int_handler:
        b .

prefetch_abt_handler:
        b .

data_abt_handler:
        b .

irq_handler:
        @b test_print

fiq_handler:
        b .

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
        b .
        @ .org MEM_REMAP_CR
        @ .byte 0x01

