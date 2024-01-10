#include <mem.h>
#include <mem_layout.h>
#include <print.h>
#include <resource.h>
#include <scheduler.h>
#include <sys_call.h>
#include <system.h>
#include <thread.h>

struct thread_control_block *const tcbs =
    (struct thread_control_block *)_INTERNAL_THREADS_TCB_ARRAY_START;

struct thread_management *const thread_management =
    (struct thread_management *)_INTERNAL_THREADS_MANAGEMENT_START;

void __create_thread(unsigned int id, int (*fun)(void *), void *input) {
  struct thread_control_block *new_tcb = &tcbs[id];

  // reset tcb content to get rid off previous content
  memset(new_tcb, 0, sizeof(struct thread_control_block));

  struct thread_context *new_ctx = &new_tcb->ctx;

  new_ctx->sp = _INTERNAL_THREADS_STACKS_START + THREAD_STACK_SIZE * (id + 1);
  new_ctx->lr = (unsigned int)sys_call_exit_thread;
  new_ctx->pc = (unsigned int)fun;
  new_ctx->cpsr = CPU_MODE_USER;
  new_ctx->registers[0] = (unsigned int)input;

  thread_management->last_created_id = id;
}

/**
 * @brief Creates a new thread with which runs the given function.
 *
 * @return 0 on success, 1 on failure (e.g. no free TCB)
 */
int thread_create(int (*fun)(void *), void *input) {
  unsigned int thread_id =
      (thread_management->last_created_id + 1) % MAX_NUM_THREADS;

  // find the next available TCB
  while (tcbs[thread_id].status != TCB_UNUSED &&
         thread_id != thread_management->last_created_id) {
    thread_id = (thread_id + 1) % MAX_NUM_THREADS;
  }

  // return a failing code if there are no free TCBs
  if (thread_id == thread_management->last_created_id) {
    return 1;
  }

  __create_thread(thread_id, fun, input);

  tcbs[thread_id].status = THREAD_READY;
  tcbs[thread_id].role = THREAD_ROLE_USER;

  return 0;
}

/**
 * @brief Marks the thread's TCB as unused.
 */
void thread_finish() {
  unsigned int finished_thread_id = thread_management->active_thread_id;
  tcbs[finished_thread_id].status = TCB_UNUSED;
}

/**
 * @brief Save the context of a given thread to its TCB.
 *
 * @param thread_id ID of the thread to be saved.
 * @param context Context to write to the TCB.
 */
void thread_save_context(unsigned int thread_id,
                         struct thread_context *context) {
  memcpy(context, &tcbs[thread_id].ctx, sizeof(struct thread_context));
}

/**
 * @brief Access the given thread's context and return a pointer to it.
 *
 * @param thread_id Id of the thread to get the context from.
 * @return Pointer to the context block.
 */
struct thread_context *thread_get_context(unsigned int thread_id) {
  return &tcbs[thread_id].ctx;
}

/**
 * @brief Creates the idle thread with the highest thread id.
 *
 * @param idle_fun The thread function of the idle thread to execute. Should not
 * return (contain an inifinite loop).
 */
void create_idle_thread(int (*idle_fun)()) {
  unsigned int id = MAX_NUM_THREADS - 1;
  __create_thread(id, idle_fun, 0);

  tcbs[id].status = THREAD_READY;
  tcbs[id].role = THREAD_ROLE_IDLE;
}

/**
 * @brief Set the current thread into sleeping mode.
 *
 * This also defines the wakeup time, but does not actually rotate the thread
 * out of execution. Which has to be done e.g. by the scheduler.
 *
 * @param duration *Minimum* sleep time in ms
 */
void thread_sleep(unsigned int duration) {
  int thread_id = thread_management->active_thread_id;

  // set the thread's status to asleep
  thread_wait(RESOURCE_WAITING_TIME);

  // register thread's sleep end time
  tcbs[thread_id].wake_up_time = thread_management->time_counter + duration;
}

/**
 * @brief Blocks the current thread with a resource that it is going to wait
 * for.
 *
 * @param blocking_resource The resource that thread is blocked by.
 */
void thread_wait(enum resource_type blocking_resource) {
  int thread_id = thread_management->active_thread_id;

  // set the thread's status to blocked
  tcbs[thread_id].status = THREAD_BLOCKED;

  // register thread's wake up action (interrupt)
  tcbs[thread_id].block_reason = blocking_resource;
}

/**
 * @brief Unblocks a thread that is blocked by the given resource.
 *
 * @param blocking_resource The resource type that is now accessible.
 * @return The id of the unblocked thread. If none was unblocked, returns -1.
 */
int thread_unblock(enum resource_type blocking_resource) {
  // TODO: dont always start to search at t = 0, instead
  // have a queue or something similar to manage the next unblocked thread
  int i;
  for (i = 0; i < MAX_NUM_THREADS; ++i) {
    // only the first matching thread is unblocked
    if (tcbs[i].status == THREAD_BLOCKED &&
        tcbs[i].block_reason == blocking_resource) {
      tcbs[i].status = THREAD_READY;
      tcbs[i].block_reason = RESOURCE_NONE;
      return i;
    }
  }

  return -1;
}

/**
 * @brief Sets the status of the given thread to ready.
 *
 * This function does not guarantee that the woken thread is executed next.
 *
 * @param thread_id Id of the thread to be woken.
 */
void thread_wakeup(unsigned int thread_id) {
  tcbs[thread_id].status = THREAD_READY;
  tcbs[thread_id].wake_up_time = 0;
}

/**
 * @brief Gets the registers from the given thread context pointer.
 *
 * @param context Pointer to a thread_context block that one wants the registers
 * from.
 * @return Pointer to the general-purpose registers in the given context.
 */
unsigned int *thread_registers_from_context(struct thread_context *context) {
  return context->registers;
}
