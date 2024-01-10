.extern system_interrupt_handler
.extern register_dump
.extern add_context_to_stack
.extern restore_context_from_stack

/**
* System interrupt handler
*
* This function saves the current context (registers, cpsr) and executes the
* C system interrupt handler. The handler might change the context (e.g. thread switch).
* Afterwards, the context is restored to proceed with the usual execution.
*/
.global handle_sys_interrupt
handle_sys_interrupt:
        push {lr}
        bl add_context_to_stack

        /* Stack looks like:
           user mode pc, lr(from above bl), cpsr, pc, lr, sp, r12, r11, ..., r1, r0(<-)*/

        mov r0, sp

        /* handle interrupt */
        bl system_interrupt_handler

        b restore_context_from_stack
