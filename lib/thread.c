#include <mem.h>
#include <mem_layout.h>
#include <print.h>
#include <scheduler.h>
#include <sys_call.h>
#include <system.h>
#include <thread.h>

volatile struct thread_control_block *const tcbs =
    (struct thread_control_block *)_INTERNAL_THREADS_TCB_ARRAY_START;

volatile struct thread_management *const management =
    (struct thread_management *)_INTERNAL_THREADS_MANAGEMENT_START;

void __create_thread(unsigned int id, int (*fun)(void *), void *input) {
  struct thread_control_block *new_tcb = &tcbs[id];

  // reset tcb content to get rid off previous content
  memset(new_tcb, 0, sizeof(struct thread_control_block));

  new_tcb->sp = _INTERNAL_THREADS_STACKS_START +
                THREAD_STACK_SIZE * id; // TODO: find good start for stack
  new_tcb->lr = (unsigned int)sys_call_exit_thread;
  new_tcb->pc = (unsigned int)fun;
  new_tcb->cpsr = CPU_MODE_USER; // TODO: find useful default value for cpsr
  new_tcb->registers[0] = (unsigned int)input;

  management->last_created_id = id;
}

/**
 * @brief Creates a new thread with which runs the given function.
 *
 * @return 0 on success, 1 on failure (e.g. no free TCB)
 */
int thread_create(int (*fun)(void *), void *input) {
  unsigned int thread_id = (management->last_created_id + 1) % MAX_NUM_THREADS;

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

void thread_finish() {
  unsigned int finished_thread_id = management->active_thread_id;
  management->status[finished_thread_id] = TCB_UNUSED;
}

void thread_save_context(unsigned int thread_id, unsigned int *context) {
  struct thread_control_block *tcb = &tcbs[thread_id];

  memcpy(context, (void *)tcb, 17 * 4);
}

void *thread_get_context(unsigned int thread_id) { return &(tcbs[thread_id]); }

void create_idle_thread(int (*idle_fun)()) {
  unsigned int id = MAX_NUM_THREADS - 1;
  __create_thread(id, idle_fun, 0);

  management->status[id] = IDLE;
}
