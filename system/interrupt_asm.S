.extern system_interrupt_handler
.extern register_dump

/*
r0 = cpu mode to access

returns pointer to stack where sp and lr are saved
*/
get_banked_registers:
  @ switch to sys mode to grab sp and lr
  mrs r2, cpsr
  bic r3, r2, #0x1f
  orr r3, r3, r0
  msr cpsr_c, r3

  mov r4, sp
  mov r5, lr

  msr cpsr_c, r2

  stmfd sp!, {r4, r5}

  mov r0, sp

  bx lr


/*
r0 = cpu mode to set registers in

Assumes, that sp points to the two banked values. Also, changes the sp like a double pop
*/
set_banked_registers:
  @ write user mode sp and lr; switch into sys mode for that
  ldmfd sp!, {r1, r4}

  mrs r2, cpsr
  bic r3, r2, #0x1f
  orr r3, r3, r0
  msr cpsr_c, r3

  mov sp, r1
  mov lr, r4

  msr cpsr_c, r2

  bx lr


.global handle_sys_interrupt
handle_sys_interrupt:
  /* save important registers; IRQ mode lr is the usr mode's pc */
  stmfd sp, {r0-r12, lr}
  sub sp, sp, #(14*4)

  /* disable interrupts */
  bl cpsr_disable_interrupts

  @ save saved psr
  mrs r4, spsr
  /* load saved pc to r2 */
  ldr r2, [sp, #(13*4)]
  @sub r2, #8

  /* push pc, cpsr */
  stmfd sp!, {r2, r4}

  mov r0, #0x1f
  bl get_banked_registers

  /* Stack looks like: (left is at higher address)
  pc, r12, r11, r10, r9, r8, r7, r6, r5, r4, r3, r2, r1, r0, cpsr, pc, lr, sp*/

  mov r0, sp

  /* handle interrupt */
  bl system_interrupt_handler

  /* Stack looks like: (left is at higher address)
  pc, r12, r11, r10, r9, r8, r7, r6, r5, r4, r3, r2, r1, r0, cpsr, pc, lr, sp*/

  /* re-enable interrupts */
  bl cpsr_enable_interrupts

  mov r0, #0x1f
  bl set_banked_registers

  @ load pc and cpsr
  ldmfd sp, {r0, r1}
  add sp, sp, #8

  msr spsr_c, r1
  mov lr, r0

  /* restore saved registers */
  ldmfd sp, {r0-r12}

  add sp, sp, #(14*4)

  subs pc, lr, #0

