#include <mem.h>
#include <mem_layout.h>
#include <print.h>
#include <scheduler.h>
#include <system.h>
#include <thread.h>

// these are defined in lib/thread.c
extern struct thread_control_block *const tcbs;
extern struct thread_management *const thread_management;

void write_context(struct thread_context *context, unsigned int thread_id) {
  // load context of the next thread
  struct thread_context *new_ctx = thread_get_context(thread_id);
  memcpy(new_ctx, context, sizeof(struct thread_control_block));
}

/**
 * @brief Replaces one thread context with the next thread's context
 *
 * @param context Pointer where to read and write the context from/to
 */
void scheduler_next(struct thread_context *context) {
  int old_thread_id = thread_management->active_thread_id;
  const unsigned int idle_id = MAX_NUM_THREADS - 1;

  if (old_thread_id == -1) {
    write_context(context, idle_id);
    thread_management->active_thread_id = idle_id;
    return;
  }

  // save old thread's context to its tcb
  // but only if they are given and the thread is active (not finished)
  if (context != 0 && tcbs[old_thread_id].status != TCB_UNUSED) {
    thread_save_context(old_thread_id, context);
  }

  // idle thread stays marked as idle and only active threads are marked as
  // ready. avoids marking threads as ready which were just put to sleep or
  // are waiting
  if (old_thread_id != idle_id && tcbs[old_thread_id].status == THREAD_ACTIVE) {
    tcbs[old_thread_id].status = THREAD_READY;
  }

  int new_thread_id = (old_thread_id + 1) % MAX_NUM_THREADS;

  // select next available thread to run
  while (tcbs[new_thread_id].status != THREAD_READY &&
         new_thread_id != old_thread_id) {
    new_thread_id = (new_thread_id + 1) % MAX_NUM_THREADS;
  }

  // no other thread available and last active thread not ready
  // execute idle thread instead
  if (new_thread_id == old_thread_id &&
      tcbs[new_thread_id].status != THREAD_READY) {
    new_thread_id = idle_id;
  }

  if (new_thread_id != idle_id) {
    tcbs[new_thread_id].status = THREAD_ACTIVE;
  }

  if (new_thread_id != old_thread_id) {
    write_context(context, new_thread_id);

    thread_management->active_thread_id = new_thread_id;
  }
}

void scheduler_init(int (*idle_fun)()) {
  thread_management->active_thread_id = -1;
  thread_management->last_created_id = -1;
  thread_management->time_counter = 0;

  create_idle_thread(idle_fun);
}

/**
 * @brief Count up the system time and wake up threads whose wakeup time was
 * exceeded.
 */
void scheduler_count_time() {
  struct thread_management *tm = thread_management;
  unsigned int last_time = tm->time_counter;
  tm->time_counter += st_get_intervall();

  if (last_time > tm->time_counter) {
    // TODO: handle time overflow...
    // this will last about 49h, so hope nobody uses this OS longer xD
  }

  int i;
  for (i = 0; i < MAX_NUM_THREADS; ++i) {
    if (tcbs[i].status == THREAD_BLOCKED &&
        tcbs[i].block_reason == RESOURCE_WAITING_TIME &&
        tcbs[i].wake_up_time < tm->time_counter) {
      thread_wakeup(i);
    }
  }
}

/**
 * @brief Switches the execution context to a given thread's context.
 *
 * @param thread_id Id of the thread to execute.
 * @param context Pointer to the context to write to.
 */
void scheduler_switch(unsigned int thread_id, struct thread_context *context) {
  unsigned int current_thread_id = thread_management->active_thread_id;

  // save old thread's context to its tcb
  // but only if they are given and the thread is active (not finished)
  if (tcbs[current_thread_id].status != TCB_UNUSED) {
    thread_save_context(current_thread_id, context);
  }

  // set ready status for current thread except if it was the idle thread
  if (current_thread_id != MAX_NUM_THREADS - 1) {
    tcbs[current_thread_id].status = THREAD_READY;
  }

  write_context(context, thread_id);
  thread_management->active_thread_id = thread_id;
}
