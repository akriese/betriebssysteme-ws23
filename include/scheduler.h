#include <thread.h>

void scheduler_end_thread();

void scheduler_next(struct thread_context *context);

void scheduler_init(int (*idle_fun)());

void scheduler_count_time();

void scheduler_switch(unsigned int thread_id, struct thread_context *context);
