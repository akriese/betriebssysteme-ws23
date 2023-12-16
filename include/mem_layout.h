#ifndef _MEMLAYOUT_H_
#define _MEMLAYOUT_H_

#include <mem_layout_asm.h>
#include <thread.h>

/**
 * Some OS buffers and storage data structures.
 *
 * We are reserving some SRAM memory area.
 */

#define _INTERNAL_STORAGE_AREA_START 0x201000

// DBGU receive buffer
#define _INTERNAL_DBGU_RECEIVE_BUFFER_START _INTERNAL_STORAGE_AREA_START
#define _INTERNAL_DBGU_RECEIVE_BUFFER_SIZE (4 * KiB) //
#define _INTERNAL_DBGU_RECEIVE_BUFFER_END                                      \
  (_INTERNAL_DBGU_RECEIVE_BUFFER_START + _INTERNAL_DBGU_RECEIVE_BUFFER_SIZE) //

// Thread control blocks
#define _INTERNAL_THREADS_TCB_ARRAY_START _INTERNAL_DBGU_RECEIVE_BUFFER_END
#define _INTERNAL_THREADS_TCB_ARRAY_SIZE                                       \
  (sizeof(struct thread_control_block) * MAX_NUM_THREADS)
#define _INTERNAL_THREADS_TCB_ARRAY_END                                        \
  (_INTERNAL_THREADS_TCB_ARRAY_START + _INTERNAL_THREADS_TCB_ARRAY_SIZE)

#define _INTERNAL_STORAGE_AREA_END 0x300000

#endif /* _MEMLAYOUT_H_ */
