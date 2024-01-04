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
  IDLE = 4,
};

struct thread_management {
  enum thread_status status[MAX_NUM_THREADS]; // 0 if thread block is free
  int active_thread_id; // id of the currently running thread
  unsigned int last_created_id;
  unsigned int time_counter;
  unsigned int wake_up_time[MAX_NUM_THREADS];
  enum resource_type block_reason[MAX_NUM_THREADS];
};

// IMPORTANT: cpsr has to come after registers due to an assumption
struct thread_control_block {
  unsigned int sp;
  unsigned int lr;
  unsigned int pc;
  unsigned int cpsr;
  unsigned int registers[13];
};

int thread_create(int (*fun)(void *), void *input);

void thread_finish();

void thread_save_context(unsigned int thread_id, void *context);

void *thread_get_context(unsigned int thread_id);

void create_idle_thread(int (*idle_fun)());

void thread_sleep(unsigned int duration);

void thread_wait(enum resource_type blocking_resource);

int thread_unblock(enum resource_type blocking_resource);

void thread_wakeup(unsigned int thread_id);

unsigned int *thread_registers_from_context(void *context);

#endif /* ifndef _THREAD_H_ */
