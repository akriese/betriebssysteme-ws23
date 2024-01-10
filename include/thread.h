#ifndef _THREAD_H_
#define _THREAD_H_

#define MAX_NUM_THREADS 32

#include <resource.h>

typedef enum thread_status {
  THREAD_READY = -1,
  TCB_UNUSED = 0,
  THREAD_ACTIVE = 1,
  THREAD_ASLEEP = 2,
  THREAD_BLOCKED = 3,
} thread_status;

typedef enum thread_role {
  THREAD_ROLE_SYSTEM,
  THREAD_ROLE_USER,
  THREAD_ROLE_IDLE,
} thread_role;

typedef struct thread_management {
  int active_thread_id; // id of the currently running thread
  unsigned int last_created_id;
  unsigned int time_counter;
} thread_management;

typedef struct thread_context {
  unsigned int registers[13];
  unsigned int sp;
  unsigned int lr;
  unsigned int pc;
  unsigned int cpsr;
} thread_context;

typedef struct thread_control_block {
  thread_context ctx;
  thread_status status;
  thread_role role;
  unsigned int wake_up_time;
  resource_type block_reason;
} thread_control_block;

int thread_create(int (*fun)(void *), void *input);

void thread_finish();

void thread_save_context(unsigned int thread_id, thread_context *context);

thread_context *thread_get_context(unsigned int thread_id);

void create_idle_thread(int (*idle_fun)());

void thread_sleep(unsigned int duration);

void thread_wait(resource_type blocking_resource);

int thread_unblock(resource_type blocking_resource);

void thread_wakeup(unsigned int thread_id);

unsigned int *thread_registers_from_context(thread_context *context);

#endif /* ifndef _THREAD_H_ */
