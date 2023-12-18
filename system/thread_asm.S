/**
*
* r0 = registers pointer
* r1 = cpsr
*/
.global scheduler_next_asm
scheduler_next_asm:
        /* choose next thread to run and return address of context to load */
        bl scheduler_next

        /* assuming the cpsr is saved "above" the registers */
        add r1, r0, #64
        ldr r1, [r1]
        msr cpsr_c, r1
        ldmfd r0, {r0-r14}

        sub pc, lr, #4

