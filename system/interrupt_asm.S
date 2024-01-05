.extern system_interrupt_handler
.extern register_dump
.extern get_banked_registers
.extern set_banked_registers
.extern restore_context

/**
* System interrupt handler
*
* This function saves the current context (registers, cpsr) and executes the
* C system interrupt handler. The handler might change the context (e.g. thread switch).
* Afterwards, the context is restored to proceed with the usual execution.
*/
.global handle_sys_interrupt
handle_sys_interrupt:
        /* save important registers; IRQ mode lr is the usr mode's pc */
        stmfd sp!, {r0-r12}

        @ save saved psr
        mrs r1, spsr
        mov r0, lr

        /* push pc, cpsr */
        stmfd sp!, {r0, r1}

        mov r0, #0x1f
        bl get_banked_registers

        /* Stack looks like: (left is at higher address)
           pc, r12, r11, r10, r9, r8, r7, r6, r5, r4, r3, r2, r1, r0, cpsr, pc, lr, sp*/

        mov r0, sp

        /* handle interrupt */
        bl system_interrupt_handler

        b restore_context

