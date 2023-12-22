#ifndef _MEMLAYOUT_H_
#define _MEMLAYOUT_H_

#include <mem_layout_asm.h>
#include <thread.h>

#define THREAD_STACK_SIZE (4 * KiB)

/**
 * Some OS buffers and storage data structures.
 *
 * We are reserving some SRAM memory area.
 */

#define _INTERNAL_STORAGE_AREA_START 0x22000000

// DBGU receive buffer
#define _INTERNAL_DBGU_RECEIVE_BUFFER_START _INTERNAL_STORAGE_AREA_START
#define _INTERNAL_DBGU_RECEIVE_BUFFER_SIZE (4 * KiB) //
#define _INTERNAL_DBGU_RECEIVE_BUFFER_END                                      \
  (_INTERNAL_DBGU_RECEIVE_BUFFER_START + _INTERNAL_DBGU_RECEIVE_BUFFER_SIZE) //

#define _INTERNAL_DBGU_THREAD_INPUT_BUFFER_START                               \
  _INTERNAL_DBGU_RECEIVE_BUFFER_END
#define _INTERNAL_DBGU_THREAD_INPUT_BUFFER_SIZE (MAX_NUM_THREADS) //
#define _INTERNAL_DBGU_THREAD_INPUT_BUFFER_END                                 \
  (_INTERNAL_DBGU_THREAD_INPUT_BUFFER_START +                                  \
   _INTERNAL_DBGU_THREAD_INPUT_BUFFER_SIZE) //

// Thread management object
#define _INTERNAL_THREADS_MANAGEMENT_START                                     \
  _INTERNAL_DBGU_THREAD_INPUT_BUFFER_END
#define _INTERNAL_THREADS_MANAGEMENT_SIZE sizeof(struct thread_management)
#define _INTERNAL_THREADS_MANAGEMENT_END                                       \
  (_INTERNAL_THREADS_MANAGEMENT_START + _INTERNAL_THREADS_MANAGEMENT_SIZE)

// Thread control blocks
#define _INTERNAL_THREADS_TCB_ARRAY_START _INTERNAL_THREADS_MANAGEMENT_END
#define _INTERNAL_THREADS_TCB_ARRAY_SIZE                                       \
  (sizeof(struct thread_control_block) * MAX_NUM_THREADS)
#define _INTERNAL_THREADS_TCB_ARRAY_END                                        \
  (_INTERNAL_THREADS_TCB_ARRAY_START + _INTERNAL_THREADS_TCB_ARRAY_SIZE)

// Thread stacks
#define _INTERNAL_THREADS_STACKS_START _INTERNAL_THREADS_TCB_ARRAY_END
#define _INTERNAL_THREADS_STACKS_SIZE (THREAD_STACK_SIZE * MAX_NUM_THREADS)
#define _INTERNAL_THREADS_STACKS_END                                           \
  (_INTERNAL_THREADS_STACKS_START + _INTERNAL_THREADS_STACKS_SIZE)

#define _INTERNAL_STORAGE_AREA_END 0x23000000

#endif /* _MEMLAYOUT_H_ */
