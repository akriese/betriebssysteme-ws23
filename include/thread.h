#ifndef _THREAD_H_
#define _THREAD_H_

#define MAX_NUM_THREADS 32

enum thread_status {
  TCB_UNUSED = 0,
  THREAD_ASLEEP = -1,
  THREAD_ACTIVE = 1,
  IDLE = 2,
};

struct thread_management {
  enum thread_status status[MAX_NUM_THREADS]; // 0 if thread block is free
  int active_thread_id; // id of the currently running thread
  unsigned int last_created_id;
};

// IMPORTANT: cpsr has to come after registers due to an assumption
struct thread_control_block {
  unsigned int sp;
  unsigned int lr;
  unsigned int pc;
  unsigned int cpsr;
  unsigned int registers[13];
  unsigned int in_use;
  unsigned int active;
  unsigned int asleep;
};

int thread_create(int (*fun)());

void thread_finish();

void thread_save_context(unsigned int thread_id, unsigned int *registers);

void *thread_get_context(unsigned int thread_id);

void create_idle_thread(int (*idle_fun)());

#endif /* ifndef _THREAD_H_ */
