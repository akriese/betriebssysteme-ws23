.global _set_cpu_mode_stack
_set_cpu_mode_stack:
        /* Read-Modify-Write to change the CPU mode */
        mrs r2, cpsr
        bic r3, r2, #0x1f
        orr r3, r3, r0
        msr cpsr_c, r3

        /* Set stack address */
        mov sp, r1

        /* Restore previous cpu mode */
        msr cpsr_c, r2

        bx lr


.global check_stack_pointer
/*
  @r0: CPU mode of interest

  @returns the stack pointer of that mode
*/
check_stack_pointer:
        /* Read-Modify-Write to change the CPU mode */
        mrs r2, cpsr
        bic r3, r2, #0x1f
        orr r3, r3, r0
        msr cpsr_c, r3

        /* Copy stack pointer to return register */
        mov r0, sp

        /* Restore previous cpu mode */
        msr cpsr_c, r2

        bx lr

.global check_cpsr
/*
  @r0: CPU mode of interest

  @returns the cpsr of that mode
*/
check_cpsr:
        mrs r0, cpsr
        bx lr

.global cpsr_enable_interrupts
cpsr_enable_interrupts:
        /* Read-Modify-Write to change the I bit */
        mrs r0, cpsr
        bic r0, r0, #0x80
        msr cpsr_c, r0

        bx lr

.global cpsr_disable_interrupts
cpsr_disable_interrupts:
        /* Read-Modify-Write to change the I bit */
        mrs r0, cpsr
        orr r0, r0, #0x80
        msr cpsr_c, r0

        bx lr

/*
r0 = cpu mode to access

returns pointer to stack where sp and lr are saved
*/
.global get_banked_registers
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
.global set_banked_registers
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

