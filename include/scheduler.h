#include <thread.h>

void scheduler_end_thread();

void scheduler_next(thread_context *context);

void scheduler_init();

void scheduler_count_time();

void scheduler_switch(unsigned int thread_id, thread_context *context);

unsigned int scheduler_count_ready();

void scheduler_set_idle_fun(int (*idle_fun)());
