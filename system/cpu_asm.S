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

/**
Save the context with all registers and cpsr to the stack.
As this is a function call, we expect, that lr was previously pushed to the stack!
ATTENTION: This function does not conform to AAPCS (sp is different after call)

The sp after this function call is basically the pointer to the context:
Stack after call: lr(user mode pc), lr, cpsr, pc, lr, sp, r12, r11,..., r1, r0

IRQ mode: lr is the user mode's pc
*/
.global add_context_to_stack
add_context_to_stack:
        /* Stack now looks like ((<-) indicates where the sp is pointing to):
          lr(user mode pc)(<-), x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x */

        /* save current lr for return at the end*/
        push {lr}

        /* Stack now looks like:
          lr(user mode pc), lr(<-), x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x */

        /*
        Decrease the stack pointer by 4 ints and push all general-purpose registers
        to later have the right layout.
        */

        sub sp, sp, #(4*4)

        /* Stack now looks like:
          lr(user mode pc), lr, x, x, x, x(<-), x, x, x, x, x, x, x, x, x, x, x, x, x, x */

        stmfd sp, {r0-r12}

        /* Stack now looks like:
          lr(user mode pc), lr, x, x, x, x(<-), r12, r11,..., r1, r0, x */

        /* go back with the stack pointer to insert cpsr, pc, lr, sp */
        add sp, sp, #(4*4)

        /* Stack now looks like:
          lr(user mode pc), lr(<-), x, x, x, x, r12, r11,..., r1, r0, x */

        @ save saved psr
        mrs r1, spsr
        ldr r0, [sp, #4] /* load the lr from the stack*/

        /* push pc (user mode), cpsr */
        stmfd sp!, {r0, r1}

        /* Stack now looks like:
          lr(user mode pc), lr, cpsr, pc(<-), x, x, r12, r11,..., r1, r0, x */

        mov r0, #0x1f
        bl get_banked_registers

        /* Stack now looks like:
          lr(user mode pc), lr, cpsr, pc, lr, sp(<-), r12, r11,..., r1, r0, x */

        /* retrieve lr for jump back */
        ldr lr, [sp, #(4*4)]

        /* Bring back sp to its correct position */
        sub sp, sp, #(4*13)

        /* Stack now looks like:
          lr(user mode pc), lr, cpsr, pc, lr, sp, r12, r11,..., r1, r0(<-), x */

        bx lr


/**
* Restores the context of a thread which currenly lies on the stack.
*
* The stack is assumed to look like: (left is at higher address)
* lr(user mode pc), lr, cpsr, pc, lr, sp, r12, r11,..., r1, r0(<-)
* Thus, call this function without link! (`b restore_context_from_stack`)
*
* At the end, lr is moved to pc to resume with the normal execution.
*/
.global restore_context_from_stack
restore_context_from_stack:
        add sp, sp, #(13*4)

        /* Stack now looks like:
          lr(user mode pc), lr, cpsr, pc, lr, sp(<-), r12, r11,..., r1, r0, x */

        mov r0, #0x1f
        bl set_banked_registers

        /* Stack now looks like:
          lr(user mode pc), lr, cpsr, pc(<-), lr, sp, r12, r11,..., r1, r0, x */

        @ load pc and cpsr
        ldmfd sp!, {r0, r1}

        msr spsr_c, r1
        mov lr, r0

        /* Stack now looks like:
          lr(user mode pc), lr(<-), cpsr, pc, lr, sp, r12, r11,..., r1, r0, x */

        sub sp, sp, #(17*4)

        /* Stack now looks like:
          lr(user mode pc), lr, cpsr, pc, lr, sp, r12, r11,..., r1, r0(<-), x */

        /* restore saved registers */
        ldmfd sp!, {r0-r12}

        /* Stack now looks like:
          lr(user mode pc), lr, cpsr, pc, lr, sp(<-), r12, r11,..., r1, r0, x */

        /* discard the other values */
        add sp, sp, #(5*4)

        movs pc, lr

