#include <mem.h>
#include <mem_layout.h>
#include <print.h>
#include <scheduler.h>
#include <system.h>
#include <thread.h>

struct thread_management *const thread_management =
    (struct thread_management *)_INTERNAL_THREADS_MANAGEMENT_START;

void scheduler_end_thread();

void write_context(void *context, unsigned int thread_id) {
  // load context of the next thread
  void *new_ctx = thread_get_context(thread_id);
  memcpy(new_ctx, context, sizeof(struct thread_control_block));
}

/**
 * @brief Replaces one thread context with the next thread's context
 *
 * @param context Pointer where to read and write the context from/to
 */
void scheduler_next(void *context) {
  int old_thread_id = thread_management->active_thread_id;
  const unsigned int idle_id = MAX_NUM_THREADS - 1;

  if (old_thread_id == -1) {
    write_context(context, idle_id);
    thread_management->active_thread_id = idle_id;
    return;
  }

  // save old thread's context to its tcb
  // but only if they are given and the thread is active (not finished)
  if (context != 0 && thread_management->status[old_thread_id] != TCB_UNUSED) {
    thread_save_context(old_thread_id, context);
  }

  // idle thread stays marked as idle and only active threads are marked as
  // ready. avoids marking threads as ready which were just put to sleep or
  // are waiting
  if (old_thread_id != idle_id &&
      thread_management->status[old_thread_id] == THREAD_ACTIVE) {
    thread_management->status[old_thread_id] = THREAD_READY;
  }

  int new_thread_id = (old_thread_id + 1) % MAX_NUM_THREADS;

  // select next available thread to run
  while (thread_management->status[new_thread_id] != THREAD_READY &&
         new_thread_id != old_thread_id) {
    new_thread_id = (new_thread_id + 1) % MAX_NUM_THREADS;
  }

  // no other thread available and last active thread not ready
  // execute idle thread instead
  if (new_thread_id == old_thread_id &&
      thread_management->status[new_thread_id] != THREAD_READY) {
    new_thread_id = idle_id;
  }

  if (new_thread_id != idle_id) {
    thread_management->status[new_thread_id] = THREAD_ACTIVE;
  }

  if (new_thread_id != old_thread_id) {
    write_context(context, new_thread_id);

    thread_management->active_thread_id = new_thread_id;
    // print("\n\r");
  }
  print("%d -> %d (%d)\n\r", old_thread_id, new_thread_id,
        thread_management->status[new_thread_id]);
}

void scheduler_start() { scheduler_next(0); }

void scheduler_init(int (*idle_fun)()) {
  thread_management->active_thread_id = -1;
  thread_management->last_created_id = -1;
  thread_management->time_counter = 0;
  memset(&thread_management->status, 0,
         sizeof(enum thread_status) * MAX_NUM_THREADS);

  create_idle_thread(idle_fun);
}

void scheduler_register_thread(unsigned int thread_id) {
  thread_management->status[thread_id] = THREAD_READY;
}

void scheduler_count_time() {
  struct thread_management *tm = thread_management;
  unsigned int last_time = tm->time_counter;
  tm->time_counter += st_get_intervall();

  if (last_time > tm->time_counter) {
    // TODO: handle time overflow...
  }

  int i;
  for (i = 0; i < MAX_NUM_THREADS; ++i) {
    if (tm->status[i] == THREAD_BLOCKED &&
        tm->block_reason[i] == RESOURCE_WAITING_TIME &&
        tm->wake_up_time[i] < tm->time_counter) {
      thread_wakeup(i);
    }
  }
}

void scheduler_switch(unsigned int thread_id, void *context) {
  unsigned int current_thread_id = thread_management->active_thread_id;

  // save old thread's context to its tcb
  // but only if they are given and the thread is active (not finished)
  if (thread_management->status[current_thread_id] != TCB_UNUSED) {
    thread_save_context(current_thread_id, context);
  }

  // set ready status for current thread except if it was the idle thread
  if (current_thread_id != MAX_NUM_THREADS - 1) {
    thread_management->status[current_thread_id] = THREAD_READY;
  }

  write_context(context, thread_id);
  thread_management->active_thread_id = thread_id;
}
