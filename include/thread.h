#ifndef _THREAD_H_
#define _THREAD_H_

#define MAX_NUM_THREADS 32

struct thread_management {
  char used[MAX_NUM_THREADS];    // 0 if thread block is free
  unsigned int active_thread_id; // id of the currently running thread
  unsigned int last_created_id;
};

struct thread_control_block {
  unsigned int registers[16];
  unsigned int cpsr;
  unsigned int in_use;
  unsigned int active;
  unsigned int asleep;
  // void *sp;
  // void *lr;
  // void *pc;
};

int thread_create(int (*fun)());

void thread_finish();

void thread_save_context(unsigned int thread_id, unsigned int *registers);

void *thread_load_context(unsigned int thread_id);

#endif /* ifndef _THREAD_H_ */
