/**
*
* r0 = registers pointer
* r1 = cpsr
*/
.global scheduler_next_asm
scheduler_next_asm:
        /* choose next thread to run and return address of context to load */
        bl scheduler_next

        ldmfd r0, {r0-r12}


