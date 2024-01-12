#include <mem.h>
#include <mem_layout.h>
#include <print.h>
#include <scheduler.h>
#include <system.h>
#include <thread.h>

// these are defined in lib/thread.c
extern thread_control_block *const tcbs;
extern thread_management *const t_management;

void write_context(thread_context *context, unsigned int thread_id) {
  // load context of the next thread
  thread_context *new_ctx = thread_get_context(thread_id);
  memcpy(new_ctx, context, sizeof(thread_context));
}

int __idle_fun(void *__unused) {
  // simply doing an infinite loop will lead to an assembly of one instruction
  // which leads to weird behaviour when the thread is started again.
  // That is why, we insert this dummy variable here
  volatile int j = 0;
  while (1) {
    j++;
  }

  return 0;
}

/**
 * @brief Replaces one thread context with the next thread's context
 *
 * @param context Pointer where to read and write the context from/to
 */
void scheduler_next(thread_context *context) {
  int old_thread_id = t_management->active_thread_id;
  const unsigned int idle_id = MAX_NUM_THREADS - 1;

  if (old_thread_id == -1) {
    write_context(context, idle_id);
    t_management->active_thread_id = idle_id;
    t_management->last_scheduled_thread = idle_id;
    return;
  }

  // save old thread's context to its tcb
  // but only if they are given and the thread is active (not finished)
  if (context != 0 && tcbs[old_thread_id].status != TCB_UNUSED) {
    thread_save_context(old_thread_id, context);
  }

  // avoids marking threads as ready which were just put to sleep or
  // are waiting
  if (tcbs[old_thread_id].status == THREAD_ACTIVE) {
    tcbs[old_thread_id].status = THREAD_READY;
  }

  // start searching the next available thread at the next position
  // This is essentially leads to a round robin scheduling
  int new_thread_id =
      (t_management->last_scheduled_thread + 1) % MAX_NUM_THREADS;

  // select next available thread to run
  // iterate as long as the thread with the id is:
  // not the previously running AND either an idle OR not ready thread
  while (new_thread_id != old_thread_id &&
         (tcbs[new_thread_id].role == THREAD_ROLE_IDLE ||
          tcbs[new_thread_id].status != THREAD_READY)) {
    new_thread_id = (new_thread_id + 1) % MAX_NUM_THREADS;
  }

  // no other thread available and last active thread not ready
  // execute idle thread instead
  if (new_thread_id == old_thread_id &&
      tcbs[new_thread_id].status != THREAD_READY) {
    new_thread_id = idle_id;
  }

  tcbs[new_thread_id].status = THREAD_ACTIVE;

  if (new_thread_id != old_thread_id) {
    write_context(context, new_thread_id);

    t_management->active_thread_id = new_thread_id;
    t_management->last_scheduled_thread = new_thread_id;
  }
}

unsigned int scheduler_count_ready() {
  int c = 0, i;

  for (i = 0; i < MAX_NUM_THREADS; ++i) {
    if (tcbs[i].status == THREAD_READY)
      c++;
  }

  return c;
}

/**
 * @brief Initialize the scheduler data structures and create the idle thread.
 *
 * @param idle_fun Function to run when no other thread is ready to run.
 */
void scheduler_init() {
  t_management->active_thread_id = -1;
  t_management->last_created_id = -1;
  t_management->time_counter = 0;

  create_idle_thread(__idle_fun);
}

void scheduler_set_idle_fun(int (*idle_fun)()) { create_idle_thread(idle_fun); }

/**
 * @brief Count up the system time and wake up threads whose wakeup time was
 * exceeded.
 */
void scheduler_count_time() {
  thread_management *tm = t_management;
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
void scheduler_switch(unsigned int thread_id, thread_context *context) {
  unsigned int current_thread_id = t_management->active_thread_id;

  // save old thread's context to its tcb
  // but only if they are given and the thread is active (not finished)
  if (tcbs[current_thread_id].status != TCB_UNUSED) {
    thread_save_context(current_thread_id, context);
  }

  tcbs[current_thread_id].status = THREAD_READY;

  write_context(context, thread_id);
  t_management->active_thread_id = thread_id;
  t_management->last_scheduled_thread = current_thread_id;
}
