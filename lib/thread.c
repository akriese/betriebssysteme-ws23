#include "resource.h"
#include <mem.h>
#include <mem_layout.h>
#include <print.h>
#include <scheduler.h>
#include <sys_call.h>
#include <system.h>
#include <thread.h>

struct thread_control_block *const tcbs =
    (struct thread_control_block *)_INTERNAL_THREADS_TCB_ARRAY_START;

struct thread_management *const management =
    (struct thread_management *)_INTERNAL_THREADS_MANAGEMENT_START;

void __create_thread(unsigned int id, int (*fun)(void *), void *input) {
  struct thread_control_block *new_tcb = &tcbs[id];

  // reset tcb content to get rid off previous content
  memset(new_tcb, 0, sizeof(struct thread_control_block));

  struct thread_context *new_ctx = &new_tcb->ctx;

  new_ctx->sp = _INTERNAL_THREADS_STACKS_START +
                THREAD_STACK_SIZE * (id + 1); // TODO: find good start for stack
  new_ctx->lr = (unsigned int)sys_call_exit_thread;
  new_ctx->pc = (unsigned int)fun;
  new_ctx->cpsr = CPU_MODE_USER; // TODO: find useful default value for cpsr
  new_ctx->registers[0] = (unsigned int)input;

  management->last_created_id = id;
}

/**
 * @brief Creates a new thread with which runs the given function.
 *
 * @return 0 on success, 1 on failure (e.g. no free TCB)
 */
int thread_create(int (*fun)(void *), void *input) {
  unsigned int thread_id = (management->last_created_id + 1) % MAX_NUM_THREADS;

  // find the next available TCB
  while (management->status[thread_id] != TCB_UNUSED &&
         thread_id != management->last_created_id) {
    thread_id = (thread_id + 1) % MAX_NUM_THREADS;
  }

  // return a failing code if there are no free TCBs
  if (thread_id == management->last_created_id) {
    return 1;
  }

  __create_thread(thread_id, fun, input);

  scheduler_register_thread(thread_id);

  return 0;
}

/**
 * @brief Marks the thread's TCB as unused.
 */
void thread_finish() {
  unsigned int finished_thread_id = management->active_thread_id;
  management->status[finished_thread_id] = TCB_UNUSED;
}

/**
 * @brief Save the context of a given thread to its TCB.
 *
 * @param thread_id ID of the thread to be saved.
 * @param context Context to write to the TCBa.
 */
void thread_save_context(unsigned int thread_id,
                         struct thread_context *context) {
  // ATTENTION: This copies as many bytes as there are in a TCB
  // If the TCB is more than just the context, this will lead to unexpected
  // behaviour.
  memcpy(context, &tcbs[thread_id].ctx, sizeof(struct thread_context));
}

struct thread_context *thread_get_context(unsigned int thread_id) {
  return &tcbs[thread_id].ctx;
}

void create_idle_thread(int (*idle_fun)()) {
  unsigned int id = MAX_NUM_THREADS - 1;
  __create_thread(id, idle_fun, 0);

  management->status[id] = IDLE;
}

void thread_sleep(unsigned int duration) {
  int thread_id = management->active_thread_id;

  // set the thread's status to asleep
  thread_wait(RESOURCE_WAITING_TIME);

  // register thread's sleep end time
  management->wake_up_time[thread_id] = management->time_counter + duration;
}

void thread_wait(enum resource_type blocking_resource) {
  int thread_id = management->active_thread_id;

  // set the thread's status to blocked
  management->status[thread_id] = THREAD_BLOCKED;

  // register thread's wake up action (interrupt)
  management->block_reason[thread_id] = blocking_resource;
}

/**
 * @brief Unblocks a thread that is blocked by the given resource.
 *
 * @param blocking_resource The resource type that is now accessible.
 * @return The id of the unblocked thread. If none was unblocked, returns -1.
 */
int thread_unblock(enum resource_type blocking_resource) {
  int thread_id = management->active_thread_id;

  int i;
  for (i = 0; i < MAX_NUM_THREADS; ++i) {
    if (management->status[i] == THREAD_BLOCKED &&
        management->block_reason[i] == blocking_resource) {
      management->status[i] = THREAD_READY;
      management->block_reason[i] = RESOURCE_NONE;
      return i;
    }
  }

  return -1;
}

void thread_wakeup(unsigned int thread_id) {
  management->status[thread_id] = THREAD_READY;
  management->wake_up_time[thread_id] = 0;
}

unsigned int *thread_registers_from_context(struct thread_context *context) {
  return context->registers;
}
