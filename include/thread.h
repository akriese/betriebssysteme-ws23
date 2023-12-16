#ifndef _THREAD_H_
#define _THREAD_H_

#define MAX_NUM_THREADS 32

struct thread_control_block {
  unsigned int registers[16];
  unsigned int in_use;
  unsigned int active;
  unsigned int asleep;
};

#endif /* ifndef _THREAD_H_ */
