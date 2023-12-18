#include <mem.h>
#include <mem_layout.h>
#include <scheduler.h>
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
  int thread_id = (management->last_created_id + 1) % MAX_NUM_THREADS;
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

  new_tcb.registers[13] = 0;
  new_tcb.registers[14] = (unsigned int)thread_finish;
  new_tcb.registers[15] = (unsigned int)fun;

  management->last_created_id = thread_id;

  return 0;
}
