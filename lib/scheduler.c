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

  // idle thread stays marked as idle
  if (thread_id != MAX_NUM_THREADS - 1) {
    thread_management->status[thread_id] = THREAD_ASLEEP;
  }

  thread_id++;

  // select next available thread to run
  while (thread_management->status[thread_id] != THREAD_ASLEEP &&
         thread_id != thread_management->active_thread_id) {
    thread_id = (thread_id + 1) % MAX_NUM_THREADS;
  }

  // no other thread available and last active thread not asleep
  // execute idle thread instead
  if (thread_id != thread_management->active_thread_id &&
      thread_management->status[thread_id] != THREAD_ASLEEP) {
    thread_id = MAX_NUM_THREADS - 1;
  }

  // load context of the next thread
  void *new_ctx = thread_get_context(thread_id);

  print("i was here: %d -> %d", thread_management->active_thread_id, thread_id);
  thread_management->active_thread_id = thread_id;
}

void scheduler_start() { scheduler_next(0); }

void scheduler_init(int (*idle_fun)()) {
  thread_management->active_thread_id = -1;
  memset(&thread_management->status, 0, MAX_NUM_THREADS * 4);
  thread_management->last_created_id = -1;

  create_idle_thread(idle_fun);
  thread_management->active_thread_id = MAX_NUM_THREADS - 1;
}

void scheduler_register_thread(unsigned int thread_id) {
  thread_management->status[thread_id] = THREAD_ASLEEP;
}
