#include <mem_layout.h>
#include <scheduler.h>
#include <thread.h>

volatile struct thread_management *const thread_management =
    (struct thread_management *)_INTERNAL_THREADS_MANAGEMENT_START;

void scheduler_register_thread();

void scheduler_end_thread();

void scheduler_next();
