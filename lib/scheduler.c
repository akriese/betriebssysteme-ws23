#include <mem.h>
#include <mem_layout.h>
#include <scheduler.h>
#include <thread.h>

volatile struct thread_management *const thread_management =
    (struct thread_management *)_INTERNAL_THREADS_MANAGEMENT_START;

void scheduler_end_thread();

void scheduler_next(unsigned int *context) {
  int thread_id = thread_management->active_thread_id;

  // save old thread's context to its tcb
  // but only if they are given
  if (context == 0) {
    thread_save_context(thread_id, context);
  }

  // select next available thread to run
  while (thread_management->used[thread_id % MAX_NUM_THREADS] == 0) {
    thread_id++;
  }

  // load context of the next thread
  void *new_ctx = thread_get_context(thread_id);

  thread_management->active_thread_id = thread_id;

  return context;
}

void scheduler_start() { scheduler_next_asm(0, 0); }

void scheduler_register_thread(unsigned int thread_id) {
  thread_management->used[thread_id] = 1;
}
