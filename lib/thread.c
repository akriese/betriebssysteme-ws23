#include <mem.h>
#include <mem_layout.h>
#include <scheduler.h>
#include <system.h>
#include <thread.h>

volatile struct thread_control_block *const tcbs =
    (struct thread_control_block *)_INTERNAL_THREADS_TCB_ARRAY_START;

volatile struct thread_management *const management =
    (struct thread_management *)_INTERNAL_THREADS_MANAGEMENT_START;
/**
 * @brief Creates a new thread with which runs the given function.
 *
 * @return 0 on success, 1 on failure (e.g. no free TCB)
 */
int thread_create(int (*fun)()) {
  unsigned int thread_id = (management->last_created_id + 1) % MAX_NUM_THREADS;
  while (management->used[thread_id] &&
         thread_id != management->last_created_id) {
    thread_id = (thread_id + 1) % MAX_NUM_THREADS;
  }

  // return a failing code if there are no free TCBs
  if (thread_id == management->last_created_id) {
    return 1;
  }

  struct thread_control_block new_tcb = tcbs[thread_id];

  // reset tcb content to get rid off previous content
  memset(&new_tcb, 0, sizeof(struct thread_management));

  new_tcb.registers[13] = 0; // TODO: find good start for stack
  new_tcb.registers[14] = (unsigned int)thread_finish;
  new_tcb.registers[15] = (unsigned int)fun;
  new_tcb.cpsr = CPU_MODE_USER; // TODO: find useful default value for cpsr

  management->last_created_id = thread_id;

  scheduler_register_thread(thread_id);

  return 0;
}

void thread_finish() {
  unsigned int finished_thread_id = management->active_thread_id;
  tcbs[finished_thread_id].in_use = 0;

  scheduler_next_asm(0, 0);
}

void thread_save_context(unsigned int thread_id, unsigned int *registers) {
  volatile struct thread_control_block *const tcb = &tcbs[thread_id];

  memcpy(registers, (void *)tcb->registers, 16);
  tcb->cpsr;
}

volatile void *thread_get_context(unsigned int thread_id) {
  return &(tcbs[thread_id]);
}
