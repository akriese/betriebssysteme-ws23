void scheduler_start();

void scheduler_register_thread(unsigned int thread_id);

void scheduler_end_thread();

void scheduler_next(unsigned int *context);
