#include <mem.h>
#include <mem_layout.h>
#include <scheduler.h>
#include <thread.h>

volatile struct thread_management *const thread_management =
    (struct thread_management *)_INTERNAL_THREADS_MANAGEMENT_START;

void scheduler_end_thread();

/**
 * @brief Replaces one thread context with the next thread's context
 *
 * @param context Pointer where to read and write the context from/to
 */
void scheduler_next(unsigned int *context) {
  int thread_id = thread_management->active_thread_id;
  const unsigned int idle_id = MAX_NUM_THREADS - 1;

  if (thread_id == -1) {
    thread_id = idle_id;
  } else {
    print("current context: ");
    print_context(context);
    // register_dump(context + 4);
    print("stored context: ");
    print_context(thread_get_context(thread_id));

    // save old thread's context to its tcb
    // but only if they are given and the thread is active (not finished)
    if (context != 0 && thread_management->status[thread_id] != TCB_UNUSED) {
      thread_save_context(thread_id, context);
    }

    // idle thread stays marked as idle
    if (thread_id != idle_id &&
        thread_management->status[thread_id] == THREAD_ACTIVE) {
      thread_management->status[thread_id] = THREAD_ASLEEP;
    }

    thread_id = (thread_id + 1) % MAX_NUM_THREADS;

    // select next available thread to run
    while (thread_management->status[thread_id] != THREAD_ASLEEP &&
           thread_id != thread_management->active_thread_id) {
      thread_id = (thread_id + 1) % MAX_NUM_THREADS;
    }

    // no other thread available and last active thread not asleep
    // execute idle thread instead
    if (thread_id == thread_management->active_thread_id &&
        thread_management->status[thread_id] != THREAD_ASLEEP) {
      thread_id = idle_id;
    }

    if (thread_id != idle_id) {
      thread_management->status[thread_id] = THREAD_ACTIVE;
    }
  }

  if (thread_id != thread_management->active_thread_id) {
    // load context of the next thread
    void *new_ctx = thread_get_context(thread_id);
    memcpy(new_ctx, context, 17 * 4);

    print("new context: ");
    print_context(context);
    // register_dump(context + 4);
    print("i was here: %d -> %d:\n\r", thread_management->active_thread_id,
          thread_id);
    thread_management->active_thread_id = thread_id;
  } else {
    print("No context switch!\n\r");
  }
}

void scheduler_start() { scheduler_next(0); }

void scheduler_init(int (*idle_fun)()) {
  thread_management->active_thread_id = -1;
  thread_management->last_created_id = -1;
  memset(&thread_management->status, 0,
         sizeof(enum thread_status) * MAX_NUM_THREADS);

  create_idle_thread(idle_fun);
}

void scheduler_register_thread(unsigned int thread_id) {
  thread_management->status[thread_id] = THREAD_ASLEEP;
}
