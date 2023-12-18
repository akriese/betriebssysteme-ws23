#include <mem_layout.h>
#include <scheduler.h>
#include <thread.h>

volatile struct thread_management *const thread_management =
    (struct thread_management *)_INTERNAL_THREADS_MANAGEMENT_START;

void scheduler_end_thread();

volatile void *scheduler_next(unsigned int *registers, unsigned int cpsr) {
  int thread_id = thread_management->active_thread_id;

  // save old thread's context to its tcb
  // but only if they are given
  if (registers == 0 && cpsr == 0) {
    thread_save_context(thread_id, registers);
  }

  // select next available thread to run
  while (thread_management->used[thread_id % MAX_NUM_THREADS] == 0) {
    thread_id++;
  }

  // load context of the next thread
  volatile void *context = thread_get_context(thread_id);

  thread_management->active_thread_id = thread_id;

  return context;
}

void scheduler_start() { scheduler_next_asm(0, 0); }

void scheduler_register_thread(unsigned int thread_id) {
  thread_management->used[thread_id] = 1;
}
