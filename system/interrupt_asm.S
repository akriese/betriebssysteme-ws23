.extern system_interrupt_handler
.extern register_dump

.global handle_sys_interrupt
handle_sys_interrupt:
  /* save important registers */
  stmfd sp!, {r0-r15}
  mov r0, sp
  bl register_dump

  /* handle interrupt */
  bl system_interrupt_handler

  /* restore saved registers */

  /* jump back, enable interrupts */
