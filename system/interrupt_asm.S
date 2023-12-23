.extern system_interrupt_handler
.extern register_dump
.extern get_banked_registers
.extern set_banked_registers

.global handle_sys_interrupt
handle_sys_interrupt:
  /* save important registers; IRQ mode lr is the usr mode's pc */
  stmfd sp!, {r0-r12, lr}

  /* disable interrupts */
  bl cpsr_disable_interrupts

  @ save saved psr
  mrs r4, spsr
  /* load saved pc to r2 */
  ldr r2, [sp, #(13*4)]

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
  ldmfd sp!, {r0, r1}

  msr spsr_c, r1
  mov lr, r0

  /* restore saved registers and set pc while switching cpu mode */
  ldmfd sp!, {r0-r12}

  movs pc, lr

