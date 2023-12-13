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
        /* Read-Modify-Write to change the CPU mode */
        mrs r2, cpsr
        bic r3, r2, #0x1f
        orr r3, r3, r0
        msr cpsr_c, r3

        /* Copy stack pointer to return register */
        mov r0, r3

        /* Restore previous cpu mode */
        msr cpsr_c, r2

        bx lr

