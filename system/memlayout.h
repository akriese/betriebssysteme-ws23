#ifndef _MEMLAYOUT_H_
#define _MEMLAYOUT_H_

/* Stacks tummeln sich am Ende des RAMs */
#define STACK_BOTTOM 0x24000000

/* 64k should be enough for everyone */
#define STACK_SIZE (1024 * 64)

#endif /* _MEMLAYOUT_H_ */
