void scheduler_start();

void scheduler_register_thread();

void scheduler_end_thread();

void *scheduler_next(unsigned int *registers, unsigned int cpsr);

void scheduler_next_asm(unsigned int *registers, unsigned int cpsr);
