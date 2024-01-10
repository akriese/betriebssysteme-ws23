#ifndef _THREAD_H_
#define _THREAD_H_

#define MAX_NUM_THREADS 32

#include <resource.h>

enum thread_status {
  THREAD_READY = -1,
  TCB_UNUSED = 0,
  THREAD_ACTIVE = 1,
  THREAD_ASLEEP = 2,
  THREAD_BLOCKED = 3,
};

enum thread_role {
  THREAD_ROLE_SYSTEM,
  THREAD_ROLE_USER,
  THREAD_ROLE_IDLE,
};

struct thread_management {
  int active_thread_id; // id of the currently running thread
  unsigned int last_created_id;
  unsigned int time_counter;
};

// IMPORTANT: cpsr has to come after registers due to an assumption
struct thread_context {
  unsigned int sp;
  unsigned int lr;
  unsigned int pc;
  unsigned int cpsr;
  unsigned int registers[13];
};

struct thread_control_block {
  struct thread_context ctx;
  enum thread_status status;
  unsigned int wake_up_time;
  enum resource_type block_reason;
  enum thread_role role;
};

int thread_create(int (*fun)(void *), void *input);

void thread_finish();

void thread_save_context(unsigned int thread_id,
                         struct thread_context *context);

struct thread_context *thread_get_context(unsigned int thread_id);

void create_idle_thread(int (*idle_fun)());

void thread_sleep(unsigned int duration);

void thread_wait(enum resource_type blocking_resource);

int thread_unblock(enum resource_type blocking_resource);

void thread_wakeup(unsigned int thread_id);

unsigned int *thread_registers_from_context(struct thread_context *context);

#endif /* ifndef _THREAD_H_ */
