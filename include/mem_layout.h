#ifndef _MEMLAYOUT_H_
#define _MEMLAYOUT_H_

/* Stacks tummeln sich am Ende des RAMs */
#define STACK_BOTTOM 0x24000000

/* 64k should be enough for everyone */
#define STACK_SIZE (1024 * 64)

/**
 * Some OS buffers and storage data structures.
 *
 * We are reserving some SRAM memory area.
 */

#define _INTERNAL_STORAGE_AREA_START 0x201000

#define _INTERNAL_DBGU_RECEIVE_BUFFER_START _INTERNAL_STORAGE_AREA_START
#define _INTERNAL_DBGU_RECEIVE_BUFFER_SIZE                                     \
  1024 // buffer size is slightly smaller

#define _INTERNAL_STORAGE_AREA_END 0x300000

#endif /* _MEMLAYOUT_H_ */
