.extern system_interrupt_handler
.extern register_dump

.global handle_sys_interrupt
handle_sys_interrupt:
  /* save important registers */
  stmfd sp!, {r0-r15}
  mov r0, sp

  /* dump registers */
  bl register_dump

  /* disable interrupts */
  bl cpsr_disable_interrupts

  /* handle interrupt */
  bl system_interrupt_handler

  /* re-enable interrupts */
  bl cpsr_enable_interrupts

  /* restore saved registers */
  ldmfd sp!, {r0-r12}

  add sp, sp, #4

  pop {lr}

  add sp, sp, #4

  subs pc, lr, #4




