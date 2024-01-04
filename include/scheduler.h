void scheduler_start();

void scheduler_register_thread(unsigned int thread_id);

void scheduler_end_thread();

void scheduler_next(void *context);

void scheduler_init(int (*idle_fun)());

void scheduler_count_time();

void scheduler_switch(unsigned int thread_id, void *context);
