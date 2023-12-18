void scheduler_start();

void scheduler_register_thread(unsigned int thread_id);

void scheduler_end_thread();

volatile void *scheduler_next(unsigned int *registers, unsigned int cpsr);

void scheduler_next_asm(unsigned int *registers, unsigned int cpsr);
